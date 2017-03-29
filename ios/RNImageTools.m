
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

@end

@implementation RNImageTools

- (dispatch_queue_t)methodQueue
{
    return dispatch_get_main_queue();
}
RCT_EXPORT_MODULE()

RCT_EXPORT_METHOD(imageMetadata:(NSString*)imageUri resolver:(RCTPromiseResolveBlock)resolve rejecter:(RCTPromiseRejectBlock)reject) {
    UIImage *image = [UIImage imageWithData:[NSData dataWithContentsOfURL:[NSURL URLWithString:imageUri]]];
    NSData *imageData = UIImageJPEGRepresentation(image, 1.0);
    CGImageSourceRef source = CGImageSourceCreateWithData((CFDataRef)imageData, NULL);
    NSMutableDictionary *imageMetadata = [(NSDictionary *) CFBridgingRelease(CGImageSourceCopyPropertiesAtIndex(source, 0, NULL)) mutableCopy];
    return resolve(imageMetadata);
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

        //no idea why this is necessary, or even  correct (but it crashes without it) :(
        dispatch_group_t group = dispatch_group_create();
        dispatch_group_enter(group);
        
        __block UIImage *image;
        __block NSMutableData *metadata = nil;

        ALAssetsLibrary *library = [[ALAssetsLibrary alloc] init];
        [library assetForURL:[NSURL URLWithString:uri] resultBlock:^(ALAsset *asset) {
            if(options[@"preserveMetadata"]) {
                metadata = [[asset defaultRepresentation] metadata];
            }
            
            image = [UIImage imageWithCGImage:[[asset defaultRepresentation] fullResolutionImage]];
            dispatch_group_leave(group);
        } failureBlock:^(NSError *error) {
            NSLog(@"Error: %@ %@", error, [error userInfo]);
            dispatch_group_leave(group);
        }];

        dispatch_async(dispatch_get_main_queue(), ^{
            // end necessary stuff here ;(
            dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
            
            if(!image) {
                return reject(@"Error", @"input image missing", nil);
            } else {
                self.originalImageMetaData = metadata;
                [self sendToEditor:image];
            }
        });
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
  
