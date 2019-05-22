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
 **************************************************************************/

#ifndef AdobeNetworkStatusHeader
#define AdobeNetworkStatusHeader

#import <Foundation/Foundation.h>

/**
 * The network reachability status.
 */

typedef NS_ENUM (NSInteger, AdobeNetworkStatus)
{
    /** Network is unreachable */
    AdobeNetworkNotReachable = 1 << 0,
    /** Network is reachable via WiFi */
    AdobeNetworkReachableViaWiFi = 1 << 1,
    /** Network is reachable via WWAN */
    AdobeNetworkReachableViaWWAN = 1 << 2
};

#endif
