/*************************************************************************
 *
 * ADOBE CONFIDENTIAL
 * ___________________
 *
 *  Copyright 2013 Adobe Systems Incorporated
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
 **************************************************************************/

#ifndef AdobeNetworkErrorHeader
#define AdobeNetworkErrorHeader

#import "AdobeError.h"

/**
 * The domain for Adobe Asset errors. When NSErrors are delivered to AdobeAsset code blocks, the 
 * domain property of the NSError will be set to the value of this string constant when the 
 * NSError is an AdobeAsset error.
 */
extern NSString *const AdobeNetworkErrorDomain;

/**
 * The following keys may appear in the NSError userInfo dictionary for errors in the 
 * AdobeNetworkErrorDomain.
 */

/**
 * @c userInfo key returning an NSNumber containing the HTTP status code returned by the request 
 * that caused the error.
 */
extern NSString *const AdobeNetworkHTTPStatusKey; 

/**
 * userInfo key returning an NSString containing the url of the request that caused the error.
 */
extern NSString *const AdobeNetworkRequestURLStringKey;

/**
 * userInfo key returning an NSDictionary containing the response headers of the request that 
 * caused the error.
 */
extern NSString *const AdobeNetworkResponseHeadersKey;

/**
 * userInfo key returning an NSData* containing the data returned from the request that caused the 
 * error.
*/
extern NSString *const AdobeNetworkResponseDataKey;

/**
 * userInfo key returning an NSString containing the HTTP debug id returned by the request that 
 * caused the error.
 */
extern NSString *const AdobeNetworkHTTPDebugIDKey;

/**
 * Error codes for the AdobeNetworkErrorDomain domain.
 */
typedef NS_ENUM (NSInteger, AdobeNetworkErrorCode)
{
    /**
     * The request cannot be completed.
     *
     * This typically means that there is something wrong with the url, the data, or the file 
     * system. Repeating the request will most likely not help.
     *
     * The userInfo property of the error often contains additional information via
     * the AdobeNetworkRequestURLKey, AdobeNetworkResponseDataKey, AdobeNetworkHTTPStatusKey, 
     * AdobeErrorResponseHeadersKey and NSUnderlyingErrorKey keys.
     */
    AdobeNetworkErrorBadRequest = 0,
    
    /**
     * This error indicates a (likely temporary) problem with the network. This could be caused by 
     * a server that is down or just too slow to respond.
     *
     * The NSUnderlyingErrorKey entry in userInfo often contains more information about the cause 
     * of this error.
     */
    AdobeNetworkErrorNetworkFailure = 1,
    
    /**
     * This error indicates that the device doesn't have a network connection (any more).
     *
     * The NSUnderlyingErrorKey entry in userInfo often contains more information about the cause 
     * of this error.
     */
    AdobeNetworkErrorOffline = 3,
    
    /**
     * This error indicates that the operation was cancelled.
     *
     * The NSUnderlyingErrorKey entry in userInfo often contains more information about the cause 
     * of this error.
     */
    AdobeNetworkErrorCancelled = 4,
    
    /**
     * The request failed due to an authentication failure, such as missing or incorrect 
     * credentials.
     */
    AdobeNetworkErrorAuthenticationFailed = 5,
    
    /**
     * The service is disconnected. This most likely happened because too many requests have failed.
     */
    AdobeNetworkErrorServiceDisconnected = 6,
    
    /**
     * A local input file does not exist
     */
    AdobeNetworkErrorFileDoesNotExist = 7,
    
    /**
     * No new requests could be enqueued because the service is in the process of processing 
     * existing requests prior to invalidation.
     */
    AdobeNetworkErrorServiceInvalidating = 8,
    
    /**
     * No Adobe Cloud specified.
     */
    AdobeNetworkErrorNoCloudSpecified = 9,
    
    /**
     * Cannot reach cloud endpoint, due to, perhaps a DNS error or other similar issues. This error 
     * is equivalent to @c kCFURLErrorCannotFindHost.
     */
    AdobeNetworkErrorCannotReachCloudEndpoint = 10,
    
    /**
     * A HTTP request was forbidden by the service.
     */
    AdobeNetworkErrorRequestForbidden = 11,
    
    /**
     * Missing JSON response.
     */
    AdobeNetworkErrorMissingJSONData = 12,
    
    /**
     * Cloud endpoint could not be reached due to a lost connection. This error is similiar to 
     * @c kCFURLErrorNetworkConnectionLost.
     */
    AdobeNetworkErrorCannotReachCloudEndpointDueToLostConnection = 13,
    
    /**
     * Cloud endpoint could not be reached due to connection timeout. This error is similar to 
     * @c kCFURLErrorTimedOut.
     */
    AdobeNetworkErrorCannotReachCloudEndpointDueToTimeout = 14
};

/**
 * Wrapper class for a network error related to Adobe services.
 */
@interface AdobeNetworkError : NSObject

/**
 * Inspects an error to see if it constitutes a user-initiated, network-request cancellation 
 * "error".
 *
 * @param error The error to inspect.
 */
+ (BOOL)isCancelError:(NSError *)error;

/**
 * Inspects an error to see if it constitutes a user-initiated, network-request file not found 
 * "error".
 *
 * @param error The error to inspect.
 */
+ (BOOL)isFileNotFoundError:(NSError *)error;

/**
 * Inspects an error to see if it constitutes a user-initiated, disconnected service error.
 *
 * @param error The error to inspect.
 */
+ (BOOL)isDisconnectedServiceError:(NSError *)error;

/**
 * Inspects an error to see if it constitutes a user-initiated, disconnected network error.
 *
 * @param error The error to inspect.
 */
+ (BOOL)isNetworkOfflineError:(NSError *)error;

@end

#endif
