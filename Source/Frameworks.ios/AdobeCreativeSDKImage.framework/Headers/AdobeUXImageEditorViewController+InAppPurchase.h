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

#import "AdobeUXImageEditorViewController.h"

NS_ASSUME_NONNULL_BEGIN

@protocol AdobeImageEditorInAppPurchaseManager;
@protocol AdobeImageEditorInAppPurchaseManagerDelegate;
@class AdobeImageEditorProduct;

DEPRECATED_MSG_ATTRIBUTE("This protocol has been deprecated and will no longer be called "
                         "except for legacy Aviary IAP integrations.")
@protocol AdobeImageEditorInAppPurchaseManagerDelegate <NSObject>
@optional

/**
 Deprecated.
 
 If implemented by the photo editor's delegate, this method will be called
 just before requesting information about products available through in-app
 purchase via the StoreKit API. If you wish to provide a custom product identifier,
 then this method should return the product identifier registered in iTunes
 Connect for the provided product.
 
 @param manager The in app purchase manager.
 @param product The product for which to return the product identifier
 registered in iTunes Connect.
 @return The product identifier registered in iTunes Connect.
 
 @see AdobeImageEditorProduct
 */
- (NSString *)inAppPurchaseManager:(id<AdobeImageEditorInAppPurchaseManager>)manager
       productIdentifierForProduct:(AdobeImageEditorProduct *)product DEPRECATED_ATTRIBUTE;

@end

/**
 This protocol is implemented by the object returned by
 [AdobeUXImageEditorViewController inAppPurchaseManager]. You should 
 call these methods to activate and deactivate in-app purchase.
 
 @see AdobeUXImageEditorViewController (InAppPurchase)
 */
@protocol AdobeImageEditorInAppPurchaseManager <NSObject>

/**
 Deprecated.
 
 A delegate for handling in-app purchase-related callbacks, including mapping
 internal product identifiers to developer-provided identifiers registered in
 iTunes Connect.
 
 @see AdobeImageEditorInAppPurchaseManagerDelegate
 */
@property (nonatomic, weak, nullable) id<AdobeImageEditorInAppPurchaseManagerDelegate> delegate DEPRECATED_ATTRIBUTE;

/**
 Indicates whether manager is observing transactions.
 
 @return YES if the in-app purchase manager is observing transactions (in-app
 purchase is enabled), NO otherwise.
 */
@property (nonatomic, assign, readonly, getter=isObservingTransactions) BOOL observingTransactions;

/**
 Call this method to start observing transactions. After calling this method,
 isObservingTransactions will return YES.
 */
- (void)startObservingTransactions;

/**
 Call this method to stop observing transactions. After calling this method,
 isObservingTransactions will return NO.
 */
- (void)stopObservingTransactions;

@end

@interface AdobeUXImageEditorViewController (InAppPurchase)

/**
 The handler object for purchasing consumable content. In order for in-app
 purchase to function correctly, you must add the object returned by this method
 as an observer of the default SKPaymentQueue. In your app delegate's
 -finishedLaunchingWithOptions: method, you should call startObservingTransactions
 on the in app purchase manager.
 
 Please see the iOS SDK In-App Purchase Guide for more information about
 in-app purchases.
 
 @see AdobeImageEditorInAppPurchaseManager
 @see AdobeImageEditorInAppPurchaseManagerDelegate
 @see [AdobeImageEditorInAppPurchaseManager startObservingTransactions] and 
 [AdobeImageEditorInAppPurchaseManager stopObservingTransactions]
 
 @return The manager.
 */
+ (id<AdobeImageEditorInAppPurchaseManager>)inAppPurchaseManager;

@end

NS_ASSUME_NONNULL_END