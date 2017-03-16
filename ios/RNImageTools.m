
#import "RNImageTools.h"
#import <AssetsLibrary/AssetsLibrary.h>
#import <AVFoundation/AVFoundation.h>
#import <Photos/Photos.h>

@interface RNImageTools ()

@property (nonatomic, strong) UIImagePickerController *picker;
@property (nonatomic, strong) RCTPromiseResolveBlock resolve;

@end

@implementation RNImageTools

- (dispatch_queue_t)methodQueue
{
    return dispatch_get_main_queue();
}
RCT_EXPORT_MODULE()

RCT_EXPORT_METHOD(openGallery:(NSDictionary*)options
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
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
    self.resolve = resolve;
    
    [self checkPhotosPermissions:^(BOOL granted) {
        if (!granted) {
            reject(@"Error", @"Photo library permissions not granted", nil);
            return;
        }
        
        showPickerViewController();
    }];
}

- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary<NSString *,id> *)info
{
    UIImage *selectedImage = [info valueForKey:UIImagePickerControllerOriginalImage];
    NSData *data = UIImageJPEGRepresentation(selectedImage, 1.0);
    
    NSString *tmpFile = [NSTemporaryDirectory() stringByAppendingPathComponent:@"temp-image.jpeg"];

    [[NSFileManager defaultManager] createFileAtPath:tmpFile contents:data attributes:nil];
    
    NSURL *fileUrl = [NSURL fileURLWithPath:tmpFile];

    self.resolve([fileUrl absoluteString]);
    
    [picker dismissModalViewControllerAnimated:YES];
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
  
