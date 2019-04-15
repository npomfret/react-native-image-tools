/*************************************************************************
 *
 * ADOBE CONFIDENTIAL
 * ___________________
 *
 *  Copyright 2014 Adobe Systems Incorporated
 *  All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of Adobe Systems Incorporated and its suppliers,
 * if any.  The intellectual and technical concepts contained
 * herein are proprietary to Adobe Systems Incorporated and its
 * suppliers and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Adobe Systems Incorporated.
 *
 **************************************************************************/

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

/**
 AdobeUXImageEditorViewController presents the Adobe Image Editor.
 */

@class AdobeUXImageEditorViewController;

/**
 Implement this protocol to be notified when the user is done using the editor.
 You are responsible for dismissing the editor when you (and/or your user) are
 finished with it.
 */
@protocol AdobeUXImageEditorViewControllerDelegate <NSObject>

/**
 Implement this method to be notified when the user presses the "Done" button.
 
 The edited image is passed via the `image` parameter. The size of this image may
 not be equivalent to the size of the input image, if the input image is larger
 than the maximum image size supported by the SDK. Currently, the maximum
 size is {1024.0, 1024.0} pixels on all devices.
 
 @param editor The photo editor controller.
 @param image The edited image.
 
 */
- (void)photoEditor:(AdobeUXImageEditorViewController *)editor finishedWithImage:(UIImage *__nullable)image;

/**
 Implement this method to be notified when the user presses the "Cancel" button.
 
 @param editor The photo editor controller.
 */
- (void)photoEditorCanceled:(AdobeUXImageEditorViewController *)editor;

@end

/**
 This class encapsulates the Adobe Image SDK photo editor. Present this view controller to
 provide the user with a fast and powerful image editor. Be sure that you don't forget to
 set the delegate property to an object that conforms to the
 AdobeUXImageEditorViewControllerDelegate protocol.
 */
@interface AdobeUXImageEditorViewController : UIViewController

/**
 Initialize the photo editor controller with an image.
 
 @param image The image to edit.
 */
- (instancetype)initWithImage:(UIImage *)image;

/**
 The photo editor's delegate.
 */
@property (nonatomic, weak, nullable) id<AdobeUXImageEditorViewControllerDelegate> delegate;

/**
 Returns whether any edits have been applied to the inputted image.
 */
@property (nonatomic, assign, readonly) BOOL isImageModified;


@end

//_______________________________________________________________________________________________
//
// Below describes the Adobe Image SDK High Resolution API
//

@protocol AdobeImageEditorRender;

/// The error domain associated with issues arising from the editor.
extern NSString *const AdobeImageEditorErrorDomain;

/// Status codes for high resolution render errors
typedef NS_ENUM(NSInteger, AdobeImageEditorHighResolutionErrorCode) {
    /// Code indicating an unknown error occurred
    AdobeImageEditorHighResolutionErrorCodeUnknown = 0,
    /// Code indicating the user cancelled the edit
    AdobeImageEditorHighResolutionErrorCodeUserCancelled,
    /// Code indicating the user made no edits in the session
    AdobeImageEditorHighResolutionErrorCodeNoModifications,
    /// Code indicating the render was cancelled by developer action
    AdobeImageEditorHighResolutionErrorCodeRenderCancelled
};

@interface AdobeUXImageEditorViewController (HighResolutionOutput)

typedef void(^AdobeImageEditorRenderCompletion)(UIImage *__nullable result, NSError *__nullable error);

/**
 Replays all actions made in the generating AdobeUXImageEditorViewController on the provided image.
 
 The provided image will be resized to fit within the `maxSize` parameter provided before any edits
 are performed.
 
 The completion block will be called when the render has finished and the `result` parameter will
 contain the edited image. If the user pressed "Cancel" or took no actions before pressing "Done",
 the `result` UIImage in the completion block will be nil and the appropriate `error` parameter
 will be provided. If the render is cancelled by developer action, then the completion block will
 be called with a nil `result` parameter and the appropriate `error` parameter.
 
 @param image The image to replay the edits on.
 @param maxSize The size to resize the input image to before replaying edits on it.
 @param completion The block to be called when the image's render is complete.
 
 @warning Calling this method from any thread other in the main thread may result in undefined behavior.
 */
- (id<AdobeImageEditorRender>)enqueueHighResolutionRenderWithImage:(UIImage *)image
                                                       maximumSize:(CGSize)maxSize
                                                        completion:(AdobeImageEditorRenderCompletion)completion;

/**
 Replays all actions made in the generating AdobeUXImageEditorViewController on the provided image.
 
 The completion block will be called when the render has finished and the `result` parameter will
 contain the edited image. If the user pressed "Cancel" or took no actions before pressing "Done",
 the `result` UIImage in the completion block will be nil and the appropriate `error` parameter
 will be provided. If the render is cancelled by developer action, then the completion block will
 be called with a nil `result` parameter and the appropriate `error` parameter.
 
 @param image The image to replay the edits on.
 @param completion The block to be called when the image's render is complete.
 
 @warning Calling this method from any thread other in the main thread may result in undefined behavior.
 */
- (id<AdobeImageEditorRender>)enqueueHighResolutionRenderWithImage:(UIImage *)image
                                                        completion:(AdobeImageEditorRenderCompletion)completion;

@end

/**
 This protocol defines the capabilities of the token object returned by the high resolution
 API. Use these methods to query about the properties of the pending render or to cancel it if necessary.
 */
@protocol AdobeImageEditorRender <NSObject>

/**
 The handler is called with an estimate of the progress in completing the render. It is guaranteed to be
 called on the main thread.
 
 @param progressHandler The block to be called with progress updates.
 
 */
@property (nonatomic, copy, nullable) void(^progressHandler)(CGFloat);

/// The size of the image that the render will output.
@property (nonatomic, assign, readonly) CGSize outputSize;

/**
 Cancels the render associated with the token.
 
 @warning Calling this method from any thread other in the main thread may result in undefined behavior.
 */
- (void)cancelRender;

@end

NS_ASSUME_NONNULL_END