## Getting started

A working project can be found in the the [example app](https://github.com/npomfret/rn-image-tools-example).

No npm package yet - edit your `package.json` file and add

    "react-native-image-tools": "git@github.com:npomfret/react-native-image-tools.git"

Then

    $ react-native link react-native-image-tools

* Get your client id, secret key and redirect uri from [abobe.io](https://www.adobe.io/console).

## For iOS

* Download the [https://creativesdk.adobe.com/downloads.html](Adobe Creative SDK) for iOS.
* Copy the required frameworks to the a `Frameworks` folder at the root of your iOS project

## For Android

* In xCode, select your apps target, and find _Embeded Binaries_, click _+_, click _Add Other_, browse to the frameworks directory and add one of them, then repeat for the other.
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
public class MainApplication extends Application implements ReactApplication {
    ...
    
    @Override
    public void onCreate() {
        ...
        MultiDex.install(getBaseContext());
        ...
    }
}
```
  