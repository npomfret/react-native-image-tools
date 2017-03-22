package com.pomocorp.rnimagetools;

import android.app.Activity;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.net.Uri;
import android.provider.MediaStore;

import com.adobe.creativesdk.aviary.AdobeImageIntent;
import com.facebook.react.bridge.BaseActivityEventListener;
import com.facebook.react.bridge.Promise;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.bridge.ReadableMap;

import java.io.File;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.List;

import static android.app.Activity.RESULT_OK;

public class RNImageToolsModule extends ReactContextBaseJavaModule {
    private static final int REQ_CODE_CSDK_IMAGE_EDITOR = 1122;
    private static final int REQ_CODE_GALLERY_PICKER = 2233;

    private final ReactApplicationContext reactContext;
    private final SelectImageListener galleryListener;
    private final SelectImageListener editorListener;

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
    public void openGallery(ReadableMap options, Promise promise) {
        galleryListener.add(promise);

        Intent galleryPickerIntent = new Intent();
        galleryPickerIntent.setType("image/*");
        galleryPickerIntent.setAction(Intent.ACTION_GET_CONTENT);

        String title = options.hasKey("title") ? options.getString("title") : "choose image";//where does this appear?

        getReactApplicationContext().startActivityForResult(Intent.createChooser(galleryPickerIntent, title), REQ_CODE_GALLERY_PICKER, null);
    }

    @ReactMethod
    public void openEditor(ReadableMap options, Promise promise) {
        editorListener.add(promise);

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
                Bitmap.CompressFormat.valueOf(options.getString("outputFormat"));
            }

            int quality = 80;
            if(options.hasKey("quality")) {
                quality = options.getInt("quality");
            }

            AdobeImageIntent.Builder builder = new AdobeImageIntent.Builder(reactContext)
                    .setData(imageUri)
                    .withOutputFormat(format)
                    .withOutputQuality(quality)
                    .withNoExitConfirmation(true);

            getReactApplicationContext().startActivityForResult(builder.build(), REQ_CODE_CSDK_IMAGE_EDITOR, null);
        } catch (Exception e) {
            promise.reject("error", "failed to start editor", e);
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

            if(data == null || resultCode != RESULT_OK) {
                reject();
                return;
            }

            Uri uri = uriFrom(data);

            if(uri == null) {
                reject();
                return;
            }

            String realPathFromURI = resolveUri(uri);

            while (!callbacks.isEmpty()) {
                callbacks.remove(0).resolve(realPathFromURI);
            }
        }

        private void reject() {
            while (!callbacks.isEmpty()) {
                callbacks.remove(0).reject("-1", "Cancelled");
            }
        }

        protected abstract Uri uriFrom(Intent data);

        public void add(Promise promise) {
            callbacks.add(promise);
        }

        String resolveUri(Uri uri) {
            /*
            content://com.android.providers.media.documents/document/image%3A519
            content://media/external/images/media/430
            ... or maybe (like from drobox)
            file:///storage/emulated/0/Android/data/com.dropbox.android/files/u3578267/scratch/testing/2015-04-26%2017.00.22.jpg
             */
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
        public EditorListener(int code) {
            super(code);
        }

        @Override
        protected Uri uriFrom(Intent data) {
            return data.getParcelableExtra(AdobeImageIntent.EXTRA_OUTPUT_URI);
        }
    }

    private static void setField(Field field, Object newValue) throws Exception {
        field.setAccessible(true);

//        Field modifiersField = Field.class.getDeclaredField("modifiers");
//        modifiersField.setAccessible(true);
//        modifiersField.setInt(field, field.getModifiers() & ~Modifier.FINAL);

        field.set(null, newValue);
        field.setAccessible(false);
    }
}