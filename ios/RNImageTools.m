
#import "RNImageTools.h"
#import <AssetsLibrary/AssetsLibrary.h>
#import <AVFoundation/AVFoundation.h>
#import <Photos/Photos.h>
#import <AdobeCreativeSDKCore/AdobeCreativeSDKCore.h>
#import "AdobeCreativeSDKImage/AdobeCreativeSDKImage.h"

@interface RNImageTools () <AdobeUXImageEditorViewControllerDelegate>

@property (nonatomic, strong) UIImagePickerController *picker;
@property (nonatomic, strong) RCTPromiseResolveBlock resolve;
@property (nonatomic, strong) RCTPromiseRejectBlock reject;
@property (nonatomic, strong) NSString* outputFormat;
@property (nonatomic, retain) NSNumber* quality;
@property (nonatomic, retain) NSMutableData* originalImageMetaData;
@property (nonatomic, strong) NSString* saveTo;

@property (nonatomic, strong) AdobeUXImageEditorViewController *controller;

+ (BOOL) assetExists:(NSURL*)assetURL;

@end

@implementation RNImageTools

- (dispatch_queue_t)methodQueue
{
    return dispatch_get_main_queue();
}
RCT_EXPORT_MODULE()

RCT_EXPORT_METHOD(imageMetadata:(NSString*)imageUri resolver:(RCTPromiseResolveBlock)resolve rejecter:(RCTPromiseRejectBlock)reject) {
    NSURL *imageURL = [NSURL URLWithString:imageUri];
    
    NSDictionary *imageMetadata;
    if([imageUri hasPrefix:@"assets-library"]) {
        NSDictionary* asset = [RNImageTools loadImageAsset :imageURL];
        imageMetadata = [asset objectForKey:@"metadata"];
    } else {
        // thanks to: http://stackoverflow.com/questions/18265760/get-exif-data-in-mac-os-development
        CGImageSourceRef imageSource = CGImageSourceCreateWithURL((CFURLRef)imageURL, NULL);
        NSDictionary *options = [NSDictionary dictionaryWithObjectsAndKeys: [NSNumber numberWithBool:NO], (NSString *)kCGImageSourceShouldCache, nil];
        CFDictionaryRef imageProperties = CGImageSourceCopyPropertiesAtIndex(imageSource, 0, ( CFDictionaryRef)options);
        CFRelease(imageSource);
        if (imageProperties) {
            imageMetadata = [NSDictionary dictionaryWithDictionary:(__bridge NSDictionary*)(imageProperties)];
        }
        CFRelease(imageProperties);
    }
    
    //hack!
    // todo : make sure all values in the dictionary (and all sub-dictionaries) are safe to return as JSON
    NSMutableDictionary* copy = [imageMetadata mutableCopy];
    [copy removeObjectForKey:@"{MakerApple}"];
    //end hack
    
    return resolve(copy);
}

RCT_EXPORT_METHOD(authorize:(NSString*)clientId clientSecret:(NSString*) clientSecret redirectUri:(NSString*) redirectUri) {
    [[AdobeUXAuthManager sharedManager] setAuthenticationParametersWithClientID:clientId
                                                                   clientSecret:clientSecret
                                                            additionalScopeList:@[AdobeAuthManagerUserProfileScope,
                                                                                  AdobeAuthManagerEmailScope,
                                                                                  AdobeAuthManagerAddressScope]];
    
    [AdobeUXAuthManager sharedManager].redirectURL = [NSURL URLWithString:redirectUri];
}

RCT_EXPORT_METHOD(selectImage:(NSDictionary*)options
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
    self.resolve = resolve;
    self.reject = reject;
    
    UIImagePickerController *picker = [[UIImagePickerController alloc] init];
    picker.modalPresentationStyle = UIModalPresentationCurrentContext;
    picker.allowsEditing = NO;
    
    void (^showPickerViewController)() = ^void() {
        dispatch_async(dispatch_get_main_queue(), ^{
            UIViewController *root = [[[[UIApplication sharedApplication] delegate] window] rootViewController];
            while (root.presentedViewController != nil) {
                root = root.presentedViewController;
            }
            [root presentViewController:picker animated:YES completion:nil];
        });
    };
    
    picker.delegate = self;
    
    [self checkPhotosPermissions:^(BOOL granted) {
        if (!granted) {
            reject(@"Error", @"Photo library permissions not granted", nil);
            return;
        }
        
        showPickerViewController();
    }];
}

RCT_EXPORT_METHOD(openEditor:(NSDictionary*)options
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
    self.resolve = resolve;
    self.reject = reject;
    
    NSString* uri = options[@"imageUri"];
    if(!uri) {
        return reject(@"error", @"imageUri not present", nil);
    }
    
    self.quality = options[@"quality"];
    if(!self.quality) {
        self.quality = @(80);
    }
    
    self.outputFormat = options[@"outputFormat"];
    if(!self.outputFormat) {
        self.outputFormat = @"JPEG";
    }
    
    self.saveTo = options[@"saveTo"];
    if(!self.saveTo) {
        self.saveTo = @"photos";
    }
    
    self.originalImageMetaData = nil;
    
    NSURL *imageURL = [NSURL URLWithString:uri];
    
    if([uri hasPrefix:@"assets-library"]) {
        
        NSDictionary* asset = [RNImageTools loadImageAsset :imageURL];
        UIImage *image = asset[@"image"];
        NSMutableData *metadata = asset[@"metadata"];
        
        if(!image) {
            return reject(@"Error", @"input image missing", nil);
        } else {
            self.originalImageMetaData = metadata;
            [self sendToEditor:image];
        }
    } else {
        NSData *imageData;
        if([uri hasPrefix:@"/"]) {
            imageData = [NSData dataWithContentsOfFile:imageURL];
        } else {
            imageData = [NSData dataWithContentsOfURL:imageURL];
        }
        
        if(!imageData) {
            return reject(@"Error", @"input image not found", nil);
        }
        
        if(options[@"preserveMetadata"]) {
            CGImageSourceRef imageSource = CGImageSourceCreateWithURL((CFURLRef)imageURL, NULL);
            self.originalImageMetaData =  [(NSDictionary *) CFBridgingRelease(CGImageSourceCopyPropertiesAtIndex(imageSource, 0, NULL)) mutableCopy];
            CFRelease(imageSource);
        }
        
        UIImage *image = [UIImage imageWithData:imageData];
        
        [self sendToEditor:image];
    }
}

// thanks to : http://www.codercowboy.com/code-synchronous-alassetlibrary-asset-existence-check/
enum { WDASSETURL_PENDINGREADS = 1, WDASSETURL_ALLFINISHED = 0};

+ (NSDictionary*) loadImageAsset:(NSURL*)assetURL {
    __block NSDictionary *outputDict = nil;
    __block NSConditionLock * albumReadLock = [[NSConditionLock alloc] initWithCondition:WDASSETURL_PENDINGREADS];
    
    //this *MUST* execute on a background thread, ALAssetLibrary tries to use the main thread and will hang if you're on the main thread.
    dispatch_async( dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        ALAssetsLibrary * assetLibrary = [[ALAssetsLibrary alloc] init];
        [assetLibrary assetForURL:assetURL
                      resultBlock:^(ALAsset *asset) {
                          ALAssetRepresentation *defaultRepresentation = [asset defaultRepresentation];
                          
                          NSMutableData *metadata = [defaultRepresentation metadata];
                          UIImage *image = [UIImage imageWithCGImage:[defaultRepresentation fullResolutionImage]];
                          long size = [defaultRepresentation size];
                          CGSize dimensions = [defaultRepresentation dimensions];
                          ALAssetOrientation orientation = [defaultRepresentation orientation];
                          NSString *filename = [defaultRepresentation filename];
                          outputDict = @{
                                         @"image": image,
                                         @"filename": filename,
                                         @"metadata": metadata,
                                         @"size": @(size),
                                         @"orientation": @(orientation),
                                         @"dimensions": @{
                                                 @"width": @(dimensions.width),
                                                 @"height": @(dimensions.height)
                                                 }
                                         };
                          
                          // notifies the lock that "all tasks are finished"
                          [albumReadLock lock];
                          [albumReadLock unlockWithCondition:WDASSETURL_ALLFINISHED];
                      } failureBlock:^(NSError *error) {
                          // error handling
                          NSLog(@"Error: %@ %@", error, [error userInfo]);
                          // notifies the lock that "all tasks are finished"
                          [albumReadLock lock];
                          [albumReadLock unlockWithCondition:WDASSETURL_ALLFINISHED];
                      }];
    });
    
    // non-busy wait for the asset read to finish (specifically until the condition is "all finished")
    [albumReadLock lockWhenCondition:WDASSETURL_ALLFINISHED];
    [albumReadLock unlock];
    
    return outputDict;
}

- (void) sendToEditor:(UIImage*)image {
    
    AdobeUXImageEditorViewController* editor = [[AdobeUXImageEditorViewController alloc] initWithImage:image];
    
    //todo: set the options
    
    [self setController:editor];
    [[self controller] setDelegate:self];
    
    dispatch_async(dispatch_get_main_queue(), ^{
        UIViewController *root = [[[[UIApplication sharedApplication] delegate] window] rootViewController];
        while (root.presentedViewController != nil) {
            root = root.presentedViewController;
        }
        [root presentViewController:[self controller] animated:YES completion:nil];
    });
}

- (void)photoEditor:(AdobeUXImageEditorViewController *)editor finishedWithImage:(UIImage *__nullable)image
{
    [self saveImage:image];
    
    [editor dismissModalViewControllerAnimated:YES];
}

- (void)photoEditorCanceled:(AdobeUXImageEditorViewController *)editor
{
    [editor dismissModalViewControllerAnimated:YES];
    
    return self.resolve([NSNull null]);
}

- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary<NSString *,id> *)info
{
    NSURL* localUrl = (NSURL *)[info valueForKey:UIImagePickerControllerReferenceURL];
    
    [picker dismissModalViewControllerAnimated:YES];
    
    self.resolve([localUrl absoluteString]);
}

// see: https://github.com/CreativeSDK/phonegap-plugin-csdk-image-editor/blob/master/src/ios/CDVImageEditor.m
- (void) saveImage:(UIImage *) image {
    
    NSData* imageData = [self processImage:image];
    
    if([self.saveTo isEqualToString:@"photos"]) {
        [[[ALAssetsLibrary alloc] init] writeImageDataToSavedPhotosAlbum:imageData metadata:self.originalImageMetaData completionBlock:^(NSURL* url, NSError* error) {
            if (error == nil) {
                //path isn't really applicable here (this is an asset uri), but left it in for backward comparability
                self.resolve([url absoluteString]);
            } else {
                self.reject(@"error", error.description, nil);
            }
        }];
    } else {
        NSString *tmpFile = [NSTemporaryDirectory() stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.%@", [[NSUUID UUID] UUIDString], self.outputFormat]];
        
        if(self.originalImageMetaData) {
            imageData = [self addImageMetatData:imageData metadata:self.originalImageMetaData];
        }
        
        [[NSFileManager defaultManager] createFileAtPath:tmpFile contents:imageData attributes:nil];
        self.resolve([[NSURL fileURLWithPath:tmpFile] absoluteString]);
    }
}

// thanks to http://stackoverflow.com/questions/19033974/how-do-you-overwrite-image-metadata
- (NSData*)addImageMetatData:(NSData*)imageData metadata:(NSMutableDictionary*)metadataAsMutable
{
    CGImageSourceRef source = CGImageSourceCreateWithData((__bridge CFDataRef) imageData, NULL);
    
    NSDictionary* metadata = (NSDictionary *) CFBridgingRelease(CGImageSourceCopyPropertiesAtIndex(source,0,NULL));
    
    NSMutableData *outputData = [NSMutableData data];
    CGImageDestinationRef destination = CGImageDestinationCreateWithData((__bridge CFMutableDataRef)outputData,CGImageSourceGetType(source),1,NULL);
    
    if(!destination) {
        NSLog(@"Error: Could not create image destination");
        return imageData;
    }
    
    CGImageDestinationAddImageFromSource(destination,source,0, (__bridge CFDictionaryRef) metadataAsMutable);
    
    BOOL success = CGImageDestinationFinalize(destination);
    if(!success) {
        NSLog(@"Error: Could not create data from image destination");
        return imageData;
    }
    
    CFRelease(destination);
    
    return outputData;
}

- (NSData*)processImage:(UIImage*)image
{
    if ([self.outputFormat isEqualToString:@"PNG"]) {
        return UIImagePNGRepresentation(image);
    } else {//assume its a jpeg, nothing else is supported
        return UIImageJPEGRepresentation(image, [self.quality floatValue] / 100.0f);
    }
}

- (NSData*)imageData:(NSURL*)imageUrl
{
    if ([self.outputFormat isEqualToString:@"PNG"]) {
        return [NSData dataWithData:UIImagePNGRepresentation([UIImage imageWithData:[NSData dataWithContentsOfURL:imageUrl]])];
    } else {//assume its a jpeg, nothing else is supported
        return [NSData dataWithData:UIImageJPEGRepresentation([UIImage imageWithData:[NSData dataWithContentsOfURL:imageUrl]], 1)];
    }
}

- (void)checkPhotosPermissions:(void(^)(BOOL granted))callback
{
    PHAuthorizationStatus status = [PHPhotoLibrary authorizationStatus];
    if (status == PHAuthorizationStatusAuthorized) {
        callback(YES);
        return;
    } else if (status == PHAuthorizationStatusNotDetermined) {
        [PHPhotoLibrary requestAuthorization:^(PHAuthorizationStatus status) {
            if (status == PHAuthorizationStatusAuthorized) {
                callback(YES);
                return;
            }
            else {
                callback(NO);
                return;
            }
        }];
    }
    else {
        callback(NO);
    }
}

@end

