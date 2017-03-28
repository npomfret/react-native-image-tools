package com.pomocorp.rnimagetools;

import android.app.Activity;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.net.Uri;
import android.provider.MediaStore;
import android.util.Log;

import com.adobe.android.common.io.FileUtils;
import com.adobe.android.common.util.IOUtils;
import com.adobe.creativesdk.aviary.AdobeImageIntent;
import com.facebook.react.bridge.BaseActivityEventListener;
import com.facebook.react.bridge.Promise;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.bridge.ReadableMap;

import org.apache.sanselan.ImageReadException;
import org.apache.sanselan.ImageWriteException;
import org.apache.sanselan.formats.jpeg.JpegImageMetadata;
import org.apache.sanselan.formats.jpeg.JpegImageParser;
import org.apache.sanselan.formats.jpeg.exifRewrite.ExifRewriter;
import org.apache.sanselan.formats.tiff.write.TiffOutputSet;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import static android.app.Activity.RESULT_CANCELED;
import static android.app.Activity.RESULT_OK;

public class RNImageToolsModule extends ReactContextBaseJavaModule {
    private static final int REQ_CODE_CSDK_IMAGE_EDITOR = 1122;
    private static final int REQ_CODE_GALLERY_PICKER = 2233;

    private final ReactApplicationContext reactContext;
    private final GalleryListener galleryListener;
    private final EditorListener editorListener;

    public RNImageToolsModule(ReactApplicationContext reactContext) {
        super(reactContext);
        this.reactContext = reactContext;

        galleryListener = new GalleryListener(REQ_CODE_GALLERY_PICKER);
        reactContext.addActivityEventListener(galleryListener);

        editorListener = new EditorListener(REQ_CODE_CSDK_IMAGE_EDITOR);
        reactContext.addActivityEventListener(editorListener);
    }

    @Override
    public String getName() {
        return "RNImageTools";
    }

    @ReactMethod
    public void authorize(String clientId, String clientSecret, String clientUri) {
        //ignored for android
    }

    @ReactMethod
    public void selectImage(ReadableMap options, Promise promise) {
        Intent galleryPickerIntent = new Intent();
        galleryPickerIntent.setType("image/*");
        galleryPickerIntent.setAction(Intent.ACTION_GET_CONTENT);

        String title = options.hasKey("title") ? options.getString("title") : "choose image";//where does this appear?

        galleryListener.add(promise);

        getReactApplicationContext().startActivityForResult(Intent.createChooser(galleryPickerIntent, title), REQ_CODE_GALLERY_PICKER, null);
    }

    @ReactMethod
    public void openEditor(ReadableMap options, Promise promise) {
        try {
            Uri imageUri;
            if(options.hasKey("imageUri"))
                imageUri = Uri.parse(options.getString("imageUri"));
            else {
                promise.reject("error", "imageUri not present");
                return;
            }

            Bitmap.CompressFormat format = Bitmap.CompressFormat.JPEG;
            if(options.hasKey("outputFormat")) {
                format = Bitmap.CompressFormat.valueOf(options.getString("outputFormat"));
            }

            int quality = 80;
            if(options.hasKey("quality")) {
                quality = options.getInt("quality");
            }

            String saveTo = "photos";
            if(options.hasKey("saveTo")) {
                saveTo = options.getString("saveTo");
            }

            TiffOutputSet originalImageMetaData = null;
            if(options.hasKey("preserveMetadata")) {
                boolean preserveMetadata = options.getBoolean("preserveMetadata");
//            if(preserveMetadata) {
//                String realFilePath = IOUtils.getRealFilePath(reactContext, imageUri);
//                JpegImageMetadata jpegImageMetadata = (JpegImageMetadata) new JpegImageParser().getMetadata(new File(realFilePath));
//                originalImageMetaData = jpegImageMetadata.getExif().getOutputSet();
//            }
            }

            editorListener.add(promise, originalImageMetaData);

            AdobeImageIntent.Builder builder = new AdobeImageIntent.Builder(reactContext)
                    .setData(imageUri)
                    .withOutputFormat(format)
                    .withOutputQuality(quality)
                    .withNoExitConfirmation(true);

            if(!saveTo.equals("photos"))
                builder.withOutput(File.createTempFile("rnimagetools.", "." + format.name(), reactContext.getCacheDir()));

            getReactApplicationContext().startActivityForResult(builder.build(), REQ_CODE_CSDK_IMAGE_EDITOR, null);
        } catch (Exception e) {
            promise.reject("error", "failed to start editor", e);
        }
    }

    private String resolveUri(Uri uri) {
            /*
            content://com.android.providers.media.documents/document/image%3A519
            content://media/external/images/media/430
            ... or maybe (like from dropbox)
            file:///storage/emulated/0/Android/data/com.dropbox.android/files/u3578267/scratch/testing/2015-04-26%2017.00.22.jpg
             */

        String scheme = uri.getScheme();
        if(scheme == null) {
            return uri.getPath();
        } else if("file".equals(scheme)) {
            return uri.getPath();
        } else if("content".equals(scheme)) {
            Cursor cursor = reactContext.getContentResolver().query(uri, new String[]{MediaStore.Images.Media.DATA}, null, null, null);

            if (cursor == null) { // not from the local database
                return uri.toString();
            } else {
                cursor.moveToFirst();
                try {
                    int idx = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
                    String filePath = cursor.getString(idx);
                    return new File(filePath).toURI().toString();
                } finally {
                    cursor.close();
                }
            }
        } else {
            return uri.toString();
        }
    }

    private abstract class SelectImageListener extends BaseActivityEventListener {
        private final List<Promise> callbacks = new ArrayList<>();
        private final int requestCode;

        public SelectImageListener(int requestCode) {
            this.requestCode = requestCode;
        }

        @Override
        public void onActivityResult(Activity activity, int requestCode, int resultCode, Intent data) {
            if (requestCode != this.requestCode)
                return;

            if(resultCode == RESULT_CANCELED) {
                //cool - they pressed the back button
            } else if (resultCode == RESULT_OK) {
                if(data == null) {
                    reject("no data");
                    return;
                }

                Uri uri = uriFrom(data);

                if(uri != null) {
                    resolve(uri.toString());
                } else {
                    reject("no output uri");
                }
            } else {
                reject("unrecognised return code: " + resultCode);
            }
        }

        void resolve(String realPathFromURI) {
            while (!callbacks.isEmpty()) {
                callbacks.remove(0).resolve(realPathFromURI);
            }
        }

        void reject(String reason) {
            while (!callbacks.isEmpty()) {
                callbacks.remove(0).reject("error", reason);
            }
        }

        protected abstract Uri uriFrom(Intent data);

        public void add(Promise promise) {
            callbacks.add(promise);
        }
    }

    private class GalleryListener extends SelectImageListener {
        public GalleryListener(int code) {
            super(code);
        }

        @Override
        protected Uri uriFrom(Intent data) {
            return data.getData();
        }
    }

    private class EditorListener extends SelectImageListener {
        private TiffOutputSet originalImageMetadata;

        public EditorListener(int code) {
            super(code);
        }

        @Override
        protected Uri uriFrom(Intent data) {
            Uri outputUri = data.getParcelableExtra(AdobeImageIntent.EXTRA_OUTPUT_URI);

//            if(outputUri != null && this.originalImageMetadata != null) {
//                String realFilePath = IOUtils.getRealFilePath(reactContext, outputUri);
//
//                try {
//                    FileOutputStream os = new FileOutputStream(realFilePath);
//                    try {
//                        new ExifRewriter().updateExifMetadataLossy(new File(realFilePath), os, originalImageMetadata);
//                    } finally {
//                        os.close();
//                    }
//                } catch (ImageReadException | IOException | ImageWriteException e) {
//                    Log.e("RNImageTools", "failed to copy original image metadata", e);
//                }
//            }

            return outputUri;
        }

        public void add(Promise promise, TiffOutputSet originalImageMetadata) {
            this.originalImageMetadata = originalImageMetadata;
            super.add(promise);
        }
    }

}