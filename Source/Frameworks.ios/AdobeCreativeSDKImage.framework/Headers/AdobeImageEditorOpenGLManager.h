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

NS_ASSUME_NONNULL_BEGIN

/**
 The Image SDK processes images using OpenGL when possible. Before this happens, some OpenGL must be loaded, which takes can some time. 
 This class provides a few class methods to help control that loading and unloading behavior.
 
 Developers may call beginOpenGLLoad to begin OpenGL loading before launching any the Image SDK. This will reduce the already very 
 fast load time down to basically nothing.
 
 Once OpenGL data is loaded, the SDK's default behavior is to retain that data for the lifetime of the app. This allows
 the SDK to provide the best user experience possible. The data is fairly small in size, and it resides in GPU memory, 
 so you likely won't see it in your profiler.
 
 Should you have a need to release the OpenGL data, the requestOpenGLDataPurge method sets an internal flag that causes OpenGL 
 data to be unloaded when it is no longer needed. If for whatever reason you change your mind about the unload, 
 cancelOpenGLDataPurgeRequest will cancel the request if data has not already been unloaded.
 
 Calls to requestOpenGLDataPurge only apply to the currently loaded OpenGL data, so if you wish to always purge, you would need to call
 requestOpenGLDataPurge every time you launch the editor. In this case a better solution would be use the `purgeGPUMemoryWhenPossible`
 class method on AdobeImageEditorCustomization, which causes Open GL data to be purged whenever possible. This option defaults to NO 
 in order to optimize performance.
 */

@interface AdobeImageEditorOpenGLManager : NSObject

/**
 Sets whether to purge GPU memory when possible
 */

+ (void)setPurgeGPUMemoryWhenPossible:(BOOL)purgeGPUMemory;
+ (BOOL)willPurgeGPUMemoryWhenPossible;

/**
 If necessary OpenGL data has not been loaded, this call begins the OpenGL load process.
 */
+ (void)beginOpenGLLoad;

/**
 Sets a flag that tells the current OpenGL data to be unloaded when possible.
 
 Calls to requestOpenGLDataPurge only apply to the currently loaded OpenGL data, so if you wish to always purge, you would need to call
 requestOpenGLDataPurge every time you launch the editor. In this case a better solution would be use the `purgeGPUMemoryWhenPossible`
 class method on AdobeImageEditorCustomization, which causes Open GL data to be purged whenever possible. This option defaults to NO
 in order to optimize performance.
 */
+ (void)requestOpenGLDataPurge;

/**
 If a call has been made to requestOpenGLDataPurge and OpenGL data has not yet been unloaded, this method causes the request to be cancelled.
 */
+ (void)cancelOpenGLDataPurgeRequest;

@end

NS_ASSUME_NONNULL_END