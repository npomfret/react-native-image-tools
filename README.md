# react-native-image-tools

Install instructions [here](./docs/install.md).

An example app is [here](https://github.com/npomfret/rn-image-tools-example).

<img src="docs/demo.gif" width="350">

### API


_RNImageTools.openEditor(options)_ returns a promise which when resolved contains a uri to the output image, or null if editing was cancelled.

```javascript
    import RNImageTools from "react-native-image-tools";

    ...

    /*
      imageUri - the input image, can be:
        path "/foo/bar/image.jpeg"
        file url "file:///foo/bar/image.jpeg"
        url "http://foo.com/bar/image.jpeg"
        asset-uri (iOS) "assets-library://asset/asset.JPG?id=foo&EXT=JPG
        content-uri (android) "content://media/external/images/foo/bar/JPEG"
    */
    
    /*
      outputFormat - can be 'JPEG' or 'PNG'
    */

    /*
      quality - integer value between 0 and 100 representing the JPEG compression %.  Ignored for PNG.
    */
    
    /*
      preserveMetadata - true or false
    */
    
    /*
      saveTo - 'photos' or 'temp'
    */

    try {
      const uri = await RNImageTools.openEditor({
        imageUri,
        outputFormat,
        quality,
        preserveMetadata,
        saveTo
      });

      console.log("edited uri", uri);
    } catch (e) {
      console.warn("error", e);
    }
```

### todo

 * preserve metadata
   * ios support for non-asset image destination
   * android support
 * support more oof the underlying Adobe API...  