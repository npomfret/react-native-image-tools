## Getting started

A working project can be found in the the [example app](https://github.com/npomfret/rn-image-tools-example).

No npm package yet...
    
    npm install react-native-image-tools@https://github.com/npomfret/react-native-image-tools.git --save
    react-native link react-native-image-tools
    
_(Optional)_ Get your Adobe ID key (client id, secret key and redirect url) for the Abode Creative SDK 
from [abobe.io](https://www.adobe.io/console/integrations/new).  It will work without it, but you will 
get a pop up warning when the image editor is launched.  **Before releasing your app** you will need 
to submit your app to Adobe for [review](https://creativesdk.zendesk.com/hc/en-us/articles/204601215-How-to-complete-the-Production-Client-ID-Request).

## For iOS

* Download the [Adobe Creative SDK](https://creativesdk.adobe.com/downloads.html) for iOS.
* Create a folder called `Frameworks` at the root of your iOS project
* Copy the required frameworks (_AdobeCreativeSDKImage.framework_ and _AdobeCreativeSDKCore.framework_) to a `Frameworks` folder at the root of your iOS project
* in xCode:
  * under _General_ > _Embeded Binaries_, click _+_, click _Add Other_, browse to the frameworks directory and add them both
  * under _Build Settings_ > _Framework Search Paths_  add `$(PROJECT_DIR)/Frameworks`
  * clean and build

## For Android

(official docs [here](https://creativesdk.adobe.com/docs/android/#/articles/gettingstarted/index.html))

* Add some maven repos to your root level `build.gradle`:

```
 allprojects {
     repositories {
         ...
         maven {
             url 'https://repo.adobe.com/nexus/content/repositories/releases/'
         }
         maven {
             url 'http://maven.localytics.com/public'
         }
     }
 }
```

* To your app `build.gradle`, add the following:

```
android {
  ...
  defaultConfig {
    ...
    manifestPlaceholders = [appPackageName: "${applicationId}"]
    multiDexEnabled true    
  }

  dexOptions {
    jumboMode true
    javaMaxHeapSize "4g"
  }

  dependencies {
    compile 'com.android.support:multidex:1.0.1'
    ...
  }
}
```

* To your `MainApplication.java` add:

```
//add IAdobeAuthClientCredentials to the implements list

public class MainApplication extends Application implements ReactApplication, IAdobeAuthClientCredentials  {
    ...
    
    @Override
    public void onCreate() {
        ...
        MultiDex.install(getBaseContext());
        AdobeCSDKFoundation.initializeCSDKFoundation(getApplicationContext());
        ...
    }

    @Override
    public String getClientID() {
        return "your client id here";
    }

    @Override
    public String getClientSecret() {
        return "your client secret here";
    }

    @Override
    public String getRedirectURI() {
        return "your client redirect here";
    }

    @Override
    public String[] getAdditionalScopesList() {
        return new String[]{"email", "profile", "address"};
    }
}
```
