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

NS_ASSUME_NONNULL_BEGIN

/**
 This class encapsulates information about non-consumable, in-app
 purchaseable products produced by Aviary and made available to users. This
 class is used in conjunction with the AdobeImageEditorInAppPurchaseManagerDelegate
 protocol methods.
 */
@interface AdobeImageEditorProduct : NSObject

@end

@interface AdobeImageEditorProduct (Compatibility)

/**
 The name of the product in English.
 
 This should match the name entered into iTunes Connect for this product.
 */
@property (nonatomic, copy, readonly) NSString *productName;

/**
 The description of the product in English.
 
 This should match the
 description entered into iTunes Connect for this product.
 */
@property (nonatomic, copy, readonly) NSString *productDescription;

/**
 The internal product identifier.
 
 This key is guaranteed to be unique among all in-app purchase product 
 identifiers used internally by the SDK. This key should *not* be entered 
 into iTunes Connect. Create a new key, and implement the 
 AdobeImageEditorInAppPurchaseManager protocol methods to pass your unique
 identifier to the SDK. See the iOS SDK In-App Purchase Guide for more 
 information.
 */
@property (nonatomic, copy, readonly) NSString *internalProductIdentifier;

@end

NS_ASSUME_NONNULL_END