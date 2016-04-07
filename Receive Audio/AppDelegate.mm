//
//  AppDelegate.m
//  Receive Audio
//
//  Created by mutexre on 01/04/16.
//
//

#import "AppDelegate.h"
#import <stdio.h>
#import <array>
#import <boost/asio.hpp>
#import <Novocaine/Novocaine.h>
#import <boost/lockfree/spsc_queue.hpp>
#import "shared.h"

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

bool handshake(tcp::socket& socket) {
    std::array<char, 128> buf;
    boost::system::error_code error;
    
    size_t len = socket.read_some(buffer(buf), error);
    printf("received \"%s\" (%ld bytes)\n", buf.data(), len);
    
    len = socket.write_some(buffer("pong"), error);
    
    return true;
}

void startAudio(const shared_ptr<RingBuffer>& ringbuffer) {
    Novocaine* audioManager = [Novocaine audioManager];
    
    __block AudioBuf buf;
    
    audioManager.outputBlock = ^(float* data, UInt32 numFrames, UInt32 numChannels) {
        //size_t samplesCount = numFrames * numChannels;
        size_t offset = 0;
        
        for (int i = 0; i < 2; i++) {
            if (ringbuffer->pop(buf)) {
                auto bufSize = buf.size();
                memcpy(data + offset, buf.data(), bufSize * sizeof(float));
                offset += buf.size();
            }
        }
    };
    
    [audioManager play];
}

int start() {
    try {
        io_service io;
        tcp::resolver resolver(io);
        tcp::resolver::query query("192.168.1.122", "8080");
        tcp::resolver::iterator endpoint_iter = resolver.resolve(query);
        tcp::socket socket(io);
        connect(socket, endpoint_iter);
        
        auto ringbuffer = make_shared<RingBuffer>();
        
        if (handshake(socket)) {
            startAudio(ringbuffer);

            boost::system::error_code error;
            AudioBuf buf(1024);
            do {
                condition_variable cv;
                size_t len = read(socket, buffer(buf), error);
                ringbuffer->push(buf);
            }
            while (!error.value());
        }
    }
    catch (std::exception& e) {
        printf("error: %s\n", e.what());
    }
    return 0;
}

@interface AppDelegate ()

@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    start();
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
