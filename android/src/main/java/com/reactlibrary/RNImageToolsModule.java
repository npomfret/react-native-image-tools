package com.reactlibrary;

import android.app.Activity;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.provider.MediaStore;

import com.adobe.creativesdk.aviary.AdobeImageIntent;
import com.facebook.react.bridge.BaseActivityEventListener;
import com.facebook.react.bridge.Promise;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class RNImageToolsModule extends ReactContextBaseJavaModule {
    private static final int REQ_CODE_CSDK_IMAGE_EDITOR = 3001;
    private static final int REQ_CODE_GALLERY_PICKER = 20;

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
    public void openGallery(Promise promise) {
        if (promise != null)
            galleryListener.add(promise);

        Intent galleryPickerIntent = new Intent();
        galleryPickerIntent.setType("image/*");
        galleryPickerIntent.setAction(Intent.ACTION_GET_CONTENT);
        getReactApplicationContext().startActivityForResult(Intent.createChooser(galleryPickerIntent, "Select an Image"), REQ_CODE_GALLERY_PICKER, null);
    }

    @ReactMethod
    public void openEditor(String imageUri, Promise promise) {
        if (promise != null)
            editorListener.add(promise);

        Intent imageEditorIntent = new AdobeImageIntent.Builder(reactContext)
                .setData(Uri.parse(imageUri))
                .build();

        getReactApplicationContext().startActivityForResult(imageEditorIntent, REQ_CODE_CSDK_IMAGE_EDITOR, null);
    }

    private abstract class SelectImageListener extends BaseActivityEventListener {
        private final List<Promise> callbacks = new ArrayList<>();
        private int requestCode;

        public SelectImageListener(int requestCode) {
            this.requestCode = requestCode;
        }

        @Override
        public void onActivityResult(Activity activity, int requestCode, int resultCode, Intent data) {
            if(requestCode != this.requestCode)
                return;

            Uri uri = uriFrom(data);

            String realPathFromURI = galleryResolve(uri);

            while (!callbacks.isEmpty()) {
                Promise promise = callbacks.remove(0);
                promise.resolve(realPathFromURI);
            }
        }

        protected abstract Uri uriFrom(Intent data);

        public void add(Promise promise) {
            callbacks.add(promise);
        }

        protected String galleryResolve(Uri uri) {
            Cursor cursor = reactContext.getContentResolver().query(uri, new String[]{MediaStore.Images.Media.DATA}, null, null, null);
            if (cursor == null) { // not from the local database
                return uri.getPath();
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
}