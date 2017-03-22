
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

@property (nonatomic, strong) AdobeUXImageEditorViewController *controller;

@end

@implementation RNImageTools

- (dispatch_queue_t)methodQueue
{
    return dispatch_get_main_queue();
}
RCT_EXPORT_MODULE()

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
    //picker.mediaTypes = @[kUTTypeImage];
    
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
    
    NSURL *imageURL = [NSURL URLWithString:uri];

    if([uri hasPrefix:@"assets-library"]){

        //no idea why this is necessary, or even  correct (but it crashes without it) :(
        dispatch_group_t group = dispatch_group_create();
        dispatch_group_enter(group);
        
        ALAssetsLibrary *library = [[ALAssetsLibrary alloc] init];
        __block UIImage *image;
        [library assetForURL:[NSURL URLWithString:uri] resultBlock:^(ALAsset *asset) {
            image = [UIImage imageWithCGImage:[[asset defaultRepresentation] fullResolutionImage]];
            dispatch_group_leave(group);
        } failureBlock:^(NSError *error) {
            NSLog(@"Error: %@ %@", error, [error userInfo]);
            reject(@"Error", [error localizedDescription], nil);
            dispatch_group_leave(group);
        }];

        dispatch_async(dispatch_get_main_queue(), ^{
            dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
            // end necessary stuff here ;(
            
            [self sendToEditor:image];
        });
    } else {
        NSData *imageData = [NSData dataWithContentsOfURL:imageURL];
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
    NSString* savedAt = [self saveImage:image];

    self.resolve(savedAt);
    
    [editor dismissModalViewControllerAnimated:YES];
}

- (void)photoEditorCanceled:(AdobeUXImageEditorViewController *)editor
{
    [editor dismissModalViewControllerAnimated:YES];

    self.reject(@"Error", @"Cancelled", nil);
}

- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary<NSString *,id> *)info
{
    UIImage* image = [info valueForKey:UIImagePickerControllerOriginalImage];
    
    NSString* savedAt = [self saveImage:image];

    self.resolve(savedAt);
    
    [picker dismissModalViewControllerAnimated:YES];
}

// see: https://github.com/CreativeSDK/phonegap-plugin-csdk-image-editor/blob/master/src/ios/CDVImageEditor.m
- (NSString*) saveImage:(UIImage *) image {
    NSData* data = [self processImage:image];
    
    NSString *tmpFile = [NSTemporaryDirectory() stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.%@", [[NSUUID UUID] UUIDString], self.outputFormat]];
    
    [[NSFileManager defaultManager] createFileAtPath:tmpFile contents:data attributes:nil];
    
    NSURL *fileUrl = [NSURL fileURLWithPath:tmpFile];
    return [fileUrl absoluteString];
}

- (NSData*)processImage:(UIImage*)image
{
    if (self.outputFormat == @"PNG") {
        return UIImagePNGRepresentation(image);
    } else {//assume its a jpeg, nothing else is supported
        return UIImageJPEGRepresentation(image, [self.quality floatValue] / 100.0f);
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
  
