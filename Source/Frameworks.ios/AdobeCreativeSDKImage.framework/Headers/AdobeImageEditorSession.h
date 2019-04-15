/*************************************************************************
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
* suppliers and are protected by all applicable intellectual property 
* laws, including trade secret and copyright laws.
* Dissemination of this information or reproduction of this material
* is strictly forbidden unless prior written permission is obtained
* from Adobe Systems Incorporated.
**************************************************************************/

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@class AdobeImageEditorContext;

/**
 Photo Editor Sessions are obtained from instances of 
 AdobeUXImageEditorViewController through the `session` property. A session
 tracks and stores all user actions taken in the 
 AdobeUXImageEditorViewController it was obtained from.
 */
@interface AdobeImageEditorSession : NSObject

@end

@interface AdobeImageEditorSession (Compatibility)

/** Specifies whether the session is still open.
 
 Value will be YES if the generating AdobeUXImageEditorViewController has
 not been dismissed.
 */
@property (nonatomic, assign, readonly, getter=isOpen) BOOL open;

/** Specifies if the session has been cancelled.
 
 Value will be YES if the user has invalided all actions by pressing
 "Cancel" in the generating AdobeUXImageEditorViewController.
 */
@property (nonatomic, assign, readonly, getter=isCancelled) BOOL cancelled;

/** Specifies whether the session contains any actions.
 
 Value will be YES if the user has modified the image in the generating
 AdobeUXImageEditorViewController.
 */
@property (nonatomic, assign, readonly, getter=isModified) BOOL modified;

/**
 Generates a new AdobeImageEditorContext.
 
 Contexts may be used to replay the session's actions on images.
 See AdobeImageEditorContext for more information.
 
 @param image The image to generate the context with.
 @return A new photo editor context.
 
 @warning Calling this method from any thread other in the main thread may result
 in undefined behavior.
 */
- (AdobeImageEditorContext *)createContextWithImage:(UIImage *)image;

/**
 Generates a new AFPhotoEditorContext with a maximum size.
 
 @param image The image to generate the context with.
 @param size The maximum size the context should render the image at.
 @return A new photo editor context that can be used to replay the session's
 actions. See AFPhotoEditorContext.
 
 @warning Calling this method from any thread other in the main thread may
 result in undefined behavior.
 */
- (AdobeImageEditorContext *)createContextWithImage:(UIImage *)image
                                            maxSize:(CGSize)size;

@end

NS_ASSUME_NONNULL_END