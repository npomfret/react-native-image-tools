
#import "RNImageTools.h"
#import <AssetsLibrary/AssetsLibrary.h>
#import <AVFoundation/AVFoundation.h>
#import <Photos/Photos.h>
#import <AdobeCreativeSDKCore/AdobeCreativeSDKCore.h>
#import "AdobeCreativeSDKImage/AdobeCreativeSDKImage.h"
#import "NSDictionary+Merge.h"
#import <MobileCoreServices/MobileCoreServices.h>

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

- (NSArray<NSString *> *)supportedEvents {
    return @[@"photoLibraryImage"];
}

RCT_EXPORT_METHOD(imageMetadata:(NSString*)imageUri resolver:(RCTPromiseResolveBlock)resolve rejecter:(RCTPromiseRejectBlock)reject) {
    NSURL *imageURL = [NSURL URLWithString:imageUri];
    
    NSDictionary *imageMetadata = [RNImageTools imageData: imageURL][@"metadata"];

    resolve(imageMetadata);
}

RCT_EXPORT_METHOD(imageData:(NSString*)imageUri resolver:(RCTPromiseResolveBlock)resolve rejecter:(RCTPromiseRejectBlock)reject) {
    NSURL *imageURL = [NSURL URLWithString:imageUri];

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        NSMutableDictionary *imageData = [RNImageTools imageData: imageURL];
        if(imageData) {
            [imageData removeObjectForKey:@"image"];
            
            dispatch_async(dispatch_get_main_queue(), ^{
                resolve(imageData);
            });
        } else {
            reject(@"Error", @"unable to get image data", nil);
        }
    });
    
}

RCT_EXPORT_METHOD(authorize:(NSString*)clientId clientSecret:(NSString*) clientSecret redirectUri:(NSString*) redirectUri) {
    [[AdobeUXAuthManager sharedManager] setAuthenticationParametersWithClientID:clientId
                                                                   clientSecret:clientSecret
                                                            additionalScopeList:@[AdobeAuthManagerUserProfileScope,
                                                                                  AdobeAuthManagerEmailScope,
                                                                                  AdobeAuthManagerAddressScope]];
    
    [AdobeUXAuthManager sharedManager].redirectURL = [NSURL URLWithString:redirectUri];
}

RCT_EXPORT_METHOD(checkImageLibraryPermission:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
    [self hasPermission:^(BOOL granted) {
        resolve(@(granted));
    }];
}

RCT_EXPORT_METHOD(requestImageLibraryPermission:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
    [self requestPermission:^(BOOL granted) {
        if(granted) {
            resolve([NSNull null]);
        } else {
            reject(@"Error", @"Photo library permissions was not granted", nil);
        }
    }];
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
    picker.mediaTypes = @[(NSString *)kUTTypeImage];//limit to images for now
    
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
    
    [self hasPermission:^(BOOL granted) {
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
        
        NSDictionary* asset = [RNImageTools imageDataFromAssetUrl :imageURL];
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
            self.originalImageMetaData =  [RNImageTools imageData: imageURL][@"metadata"];
        }
        
        [self sendToEditor:[UIImage imageWithData:imageData]];
    }
}

RCT_EXPORT_METHOD(loadThumbnails:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
    [self loadThumbnails:0 count:10];
}

+ (NSMutableDictionary*) imageData:(NSURL*) imageURL {
    if([[imageURL scheme] hasPrefix:@"assets-library"]) {
        return [RNImageTools imageDataFromAssetUrl: imageURL];
    } else {
        return [RNImageTools imageDataFromUrl: imageURL];
    }
}


+ (NSMutableDictionary*) imageDataFromUrl:(NSURL*) imageURL {
    //todo: maybe use Rn imageLoader loadImageWithURLRequest to load the image
    NSError *error;
    NSDictionary *data = [NSData dataWithContentsOfURL: imageURL options:nil error:&error];
    if(error) {
        NSLog(@"unable to download image from %@: %@", imageURL, error.description);
        return nil;
    }
    
    NSMutableDictionary* imageData = [RNImageTools imageDataFrom:data];
    
    imageData[@"uri"] = [imageURL absoluteString];
    imageData[@"filename"] = imageURL.lastPathComponent;
    
    return imageData;
}

+ (NSMutableDictionary*) imageDataFrom:(NSData*) image {
    CGImageSourceRef imageSource = CGImageSourceCreateWithData((__bridge CFDataRef)image, NULL);
    NSMutableDictionary* imageData = [RNImageTools imageDataFromICGImageSourceRef:imageSource];
    CFRelease(imageSource);
    
    imageData[@"mimeType"] = [RNImageTools mimeTypeByGuessingFromData: image];
    imageData[@"size"] = @([image length]);
    
    return imageData;
}

+ (NSMutableDictionary*) imageDataFromICGImageSourceRef:(CGImageSourceRef) imageSource {
    // thanks to: http://stackoverflow.com/questions/18265760/get-exif-data-in-mac-os-development
    NSDictionary *options = [NSDictionary dictionaryWithObjectsAndKeys: [NSNumber numberWithBool:NO], (NSString *)kCGImageSourceShouldCache, nil];
    CFDictionaryRef imageProperties = CGImageSourceCopyPropertiesAtIndex(imageSource, 0, (CFDictionaryRef)options);
    
    NSDictionary *imageMetadata;
    NSDictionary *dimensions;
    int orientation = 0;
    NSDictionary *location;
    NSString *timestamp;
    
    if (imageProperties) {
        NSNumber *width = (NSNumber *)CFDictionaryGetValue(imageProperties, kCGImagePropertyPixelWidth);
        NSNumber *height = (NSNumber *)CFDictionaryGetValue(imageProperties, kCGImagePropertyPixelHeight);
        dimensions = @{
                       @"width": width,
                       @"height": height
                       };
        
        orientation = [(NSNumber *)CFDictionaryGetValue(imageProperties, kCGImagePropertyOrientation) intValue];
        
        imageMetadata = [NSDictionary dictionaryWithDictionary:(__bridge NSDictionary*)(imageProperties)];
        
        CFDictionaryRef exifDic = CFDictionaryGetValue(imageProperties, kCGImagePropertyExifDictionary);
        if (exifDic){
            timestamp = (NSString *)CFDictionaryGetValue(exifDic, kCGImagePropertyExifDateTimeOriginal);
        }

        CFDictionaryRef gps = CFDictionaryGetValue(imageProperties, kCGImagePropertyGPSDictionary);
        if (gps) {
            //could use these for timestamp?
            NSString *gpsTimeStamp = (NSString *)CFDictionaryGetValue(gps, kCGImagePropertyGPSTimeStamp);
            NSString *gpsDateStamp = (NSString *)CFDictionaryGetValue(gps, kCGImagePropertyGPSDateStamp);
            
            if(!timestamp && gpsTimeStamp && gpsDateStamp) {
                NSMutableString *date = [[NSMutableString alloc]init];
                
                [date appendString:gpsDateStamp];
                [date appendString:@" "];
                [date appendString:gpsTimeStamp];
                
                timestamp = date;
            }
            
            double latitude = [(NSString *)CFDictionaryGetValue(gps, kCGImagePropertyGPSLatitude) doubleValue];
            NSString *latitudeRef = (NSString *)CFDictionaryGetValue(gps, kCGImagePropertyGPSLatitudeRef);
            
            double longitude = [(NSString *)CFDictionaryGetValue(gps, kCGImagePropertyGPSLongitude) doubleValue];
            NSString *longitudeRef = (NSString *)CFDictionaryGetValue(gps, kCGImagePropertyGPSLongitudeRef);
            
            double altitude = [(NSString *)CFDictionaryGetValue(gps, kCGImagePropertyGPSAltitude) doubleValue];
            double speed = [(NSString *)CFDictionaryGetValue(gps, kCGImagePropertyGPSSpeed) doubleValue];
            double heading = [(NSString *)CFDictionaryGetValue(gps, kCGImagePropertyGPSDestBearing) doubleValue];
            
            location = @{
                         @"latitude": @([latitudeRef isEqualToString:@"S"] ? -latitude : latitude),
                         @"longitude": @([longitudeRef isEqualToString:@"W"] ? -longitude : longitude),
                         @"altitude": @(altitude),
                         @"heading": @(heading),
                         @"speed": @(speed),
                         };
        }
        
        CFRelease(imageProperties);
    }
    
    return [NSMutableDictionary dictionaryWithDictionary:@{
                                                           @"metadata": imageMetadata ? [RNImageTools jsonSafe:imageMetadata] : @{},
                                                           @"orientation": @(orientation),
                                                           @"timestamp": timestamp ? timestamp : @"",
                                                           @"location": location ? location: @{},
                                                           @"dimensions": dimensions ? dimensions : @{}
                                                           }];
}

+ (NSMutableDictionary*) toDictionary:(PHAsset *)asset imageData:(NSData *)imageData uti:(NSString *)uti orientation:(UIImageOrientation) orientation info:(NSDictionary *)info {
    NSDate *creationDate = [asset creationDate];
    NSISO8601DateFormatter *dateFormatter = [[NSISO8601DateFormatter alloc] init];
    NSString *timestamp = [dateFormatter stringFromDate:creationDate];
    
    CLLocation *loc = [asset location];
    
    NSLog(@"requestImageDataForAsset returned info(%@)", info);
    CGFloat size = (CGFloat)imageData.length;
    UIImage *image = [UIImage imageWithData:imageData];
    
    CIImage *ciImage = [CIImage imageWithData:imageData];
    NSDictionary<NSString *,id> *metadata = ciImage.properties;
    
    NSString *mimeType = nil;
    //see https://developer.apple.com/library/content/documentation/Miscellaneous/Reference/UTIRef/Articles/System-DeclaredUniformTypeIdentifiers.html
    if([uti isEqualToString:(__bridge NSString *)kUTTypePNG]) {
        mimeType = @"image/png";
    } else if ([uti isEqualToString:(__bridge NSString *)kUTTypeJPEG]) {
        mimeType = @"image/jpeg";
    } else if ([uti isEqualToString:(__bridge NSString *)kUTTypeGIF]) {
        mimeType = @"image/gif";
    } else {
        //todo... maybe try the filename?
    }
    
    return [NSMutableDictionary dictionaryWithDictionary:@{
                                                           @"image": image,
                                                           @"mimeType": mimeType,
                                                           @"metadata": [RNImageTools jsonSafe:metadata],
                                                           @"size": @(size),
                                                           @"orientation": @(orientation),
                                                           @"timestamp": timestamp,
                                                           @"location": loc ? @{
                                                               @"latitude": @(loc.coordinate.latitude),
                                                               @"longitude": @(loc.coordinate.longitude),
                                                               @"altitude": @(loc.altitude),
                                                               @"heading": @(loc.course),
                                                               @"speed": @(loc.speed),
                                                               } : @{},
                                                           @"dimensions": @{
                                                                   @"width": @(asset.pixelWidth),
                                                                   @"height": @(asset.pixelHeight)
                                                                   }
                                                           }];
}

- (void) loadThumbnails:(NSInteger*)from count:(NSInteger*) count {
    PHFetchOptions *fetchOptions = [[PHFetchOptions alloc] init];
    fetchOptions.sortDescriptors = @[[NSSortDescriptor sortDescriptorWithKey:@"creationDate" ascending:NO]];
    
    PHFetchResult *fetchResult = [PHAsset fetchAssetsWithMediaType:PHAssetMediaTypeImage options:fetchOptions];
    
    PHImageRequestOptions *requestOptions = [[PHImageRequestOptions alloc] init];
    requestOptions.deliveryMode = PHImageRequestOptionsDeliveryModeFastFormat;
    requestOptions.networkAccessAllowed = YES;
    requestOptions.resizeMode = PHImageRequestOptionsResizeModeExact;
    
    PHCachingImageManager *manager = [[PHCachingImageManager alloc] init];
    
    CGSize cellSize = CGSizeMake(100, 100);
    dispatch_group_t requestGroup = dispatch_group_create();
    
    [self sendEventWithName:@"photoLibraryImage" body:@{}];
    
    int index = 0;
    int max = 10;
    
    for (PHAsset *asset in fetchResult) {
        if(index < from) {
            continue;
        }
        
        if(index >= max) {
            return;
        }
        
        index++;
        [manager requestImageDataForAsset:asset options:requestOptions resultHandler:^(NSData *imageData, NSString *uti, UIImageOrientation orientation, NSDictionary *info) {
            NSMutableDictionary *outputDict = [[RNImageTools toDictionary:asset imageData:imageData uti:uti orientation:orientation info:info] mutableCopy];
            [outputDict removeObjectForKey:@"image"];
            
            CIImage* image = [CIImage imageWithData:imageData];
            NSUInteger length = imageData.length;
            
            NSString *id = asset.localIdentifier;
            NSRange range = [id rangeOfString:@"/"];
            NSString *newString = [id substringToIndex:range.location];
            NSString *assetUri = [NSString stringWithFormat:@"%@%@%@",@"assets-library://asset/asset.JPG?id=",newString,@"&ext=JPG"];
            outputDict[@"uri"] = assetUri;
            
            [self sendEventWithName:@"photoLibraryImage" body:outputDict];
        }];
    }
}

+ (NSMutableDictionary*) imageDataFromAssetUrl:(NSURL*)assetURL {
    PHAsset *asset = [[PHAsset fetchAssetsWithALAssetURLs:@[assetURL] options:nil] lastObject];
    
    PHImageRequestOptions *requestOptions = [[PHImageRequestOptions alloc] init];
    requestOptions.synchronous = YES;
    
    PHCachingImageManager *manager = [[PHCachingImageManager alloc] init];
    
    __block NSMutableDictionary *outputDict;
    [manager requestImageDataForAsset:asset options:requestOptions resultHandler:^(NSData *imageData, NSString *uti, UIImageOrientation orientation, NSDictionary *info) {
        outputDict = [RNImageTools toDictionary:asset imageData:imageData uti:uti orientation:orientation info:info];
        outputDict[@"uri"] = [assetURL absoluteString];
    }];
    
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
    NSDictionary* asset = [RNImageTools imageDataFromAssetUrl:localUrl];
    
    NSMutableDictionary* copy = [asset mutableCopy];
    [copy removeObjectForKey:@"metadata"];
    [copy removeObjectForKey:@"image"];
    copy[@"uri"] = [localUrl absoluteString];
    
    [picker dismissModalViewControllerAnimated:YES];
    
    self.resolve(copy);
}

// see: https://github.com/CreativeSDK/phonegap-plugin-csdk-image-editor/blob/master/src/ios/CDVImageEditor.m
- (void) saveImage:(UIImage *) image {
    
    NSData* data;
    if ([self.outputFormat isEqualToString:@"PNG"]) {
        data = UIImagePNGRepresentation(image);
    } else {//assume its a jpeg, nothing else is supported
        data = UIImageJPEGRepresentation(image, [self.quality floatValue] / 100.0f);
    }
    
    NSDictionary* metadata = [RNImageTools imageDataFrom: data][@"metadata"];
    if(self.originalImageMetaData) {
        metadata = [metadata dictionaryByMergingWith: self.originalImageMetaData];
    }
    
    if([self.saveTo isEqualToString:@"photos"]) {
        
        [[[ALAssetsLibrary alloc] init] writeImageDataToSavedPhotosAlbum:data metadata:metadata completionBlock:^(NSURL* url, NSError* error) {
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
            data = [self addImageMetatData:data metadata:metadata];
        }
        
        [[NSFileManager defaultManager] createFileAtPath:tmpFile contents:data attributes:nil];
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

- (void)requestPermission:(void(^)(BOOL granted))callback
{
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

- (void)hasPermission:(void(^)(BOOL granted))callback
{
    PHAuthorizationStatus status = [PHPhotoLibrary authorizationStatus];
    if (status == PHAuthorizationStatusAuthorized) {
        callback(YES);
    } else {
        callback(NO);
    }
}

+ (NSMutableDictionary*) jsonSafe:(NSDictionary*) metadata {
    NSMutableDictionary *copy = [metadata mutableCopy];
    //hack!
    // todo : make sure all values in the dictionary (and all sub-dictionaries) are safe to return as JSON
    [copy removeObjectForKey:@"{MakerApple}"];//this one isn't, so lets just discard it
    //end hack
    return copy;
}

+ (NSString *)mimeTypeByGuessingFromData:(NSData *)data {
    //http://stackoverflow.com/questions/4147311/finding-image-type-from-nsdata-or-uiimage
    
    char bytes[12] = {0};
    [data getBytes:&bytes length:12];
    
    const char bmp[2] = {'B', 'M'};
    const char gif[3] = {'G', 'I', 'F'};
    const char swf[3] = {'F', 'W', 'S'};
    const char swc[3] = {'C', 'W', 'S'};
    const char jpg[3] = {0xff, 0xd8, 0xff};
    const char psd[4] = {'8', 'B', 'P', 'S'};
    const char iff[4] = {'F', 'O', 'R', 'M'};
    const char webp[4] = {'R', 'I', 'F', 'F'};
    const char ico[4] = {0x00, 0x00, 0x01, 0x00};
    const char tif_ii[4] = {'I','I', 0x2A, 0x00};
    const char tif_mm[4] = {'M','M', 0x00, 0x2A};
    const char png[8] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
    const char jp2[12] = {0x00, 0x00, 0x00, 0x0c, 0x6a, 0x50, 0x20, 0x20, 0x0d, 0x0a, 0x87, 0x0a};
    
    
    if (!memcmp(bytes, bmp, 2)) {
        return @"image/x-ms-bmp";
    } else if (!memcmp(bytes, gif, 3)) {
        return @"image/gif";
    } else if (!memcmp(bytes, jpg, 3)) {
        return @"image/jpeg";
    } else if (!memcmp(bytes, psd, 4)) {
        return @"image/psd";
    } else if (!memcmp(bytes, iff, 4)) {
        return @"image/iff";
    } else if (!memcmp(bytes, webp, 4)) {
        return @"image/webp";
    } else if (!memcmp(bytes, ico, 4)) {
        return @"image/vnd.microsoft.icon";
    } else if (!memcmp(bytes, tif_ii, 4) || !memcmp(bytes, tif_mm, 4)) {
        return @"image/tiff";
    } else if (!memcmp(bytes, png, 8)) {
        return @"image/png";
    } else if (!memcmp(bytes, jp2, 12)) {
        return @"image/jp2";
    }
    
    return @"application/octet-stream"; // default type
    
}

@end

