package com.reactlibrary;

import android.app.Activity;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.provider.MediaStore;

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
    private final SelectImageListener selectImageListener;

    public RNImageToolsModule(ReactApplicationContext reactContext) {
        super(reactContext);
        this.reactContext = reactContext;

        selectImageListener = new SelectImageListener();
        reactContext.addActivityEventListener(selectImageListener);

//        AdobeCSDKFoundation.initializeCSDKFoundation(reactContext);
    }

    @Override
    public String getName() {
        return "RNImageTools";
    }

    @ReactMethod
    public void openGallery(Promise promise) {
        if (promise != null)
            selectImageListener.add(promise);

        Intent galleryPickerIntent = new Intent();
        galleryPickerIntent.setType("image/*");
        galleryPickerIntent.setAction(Intent.ACTION_GET_CONTENT);
        getReactApplicationContext().startActivityForResult(Intent.createChooser(galleryPickerIntent, "Select an Image"), REQ_CODE_GALLERY_PICKER, null);
    }

    @ReactMethod
    public void openEditor(String imageUri, Promise promise) {

    }

    private class SelectImageListener extends BaseActivityEventListener {
        private final List<Promise> callbacks = new ArrayList<>();

        @Override
        public void onActivityResult(Activity activity, int requestCode, int resultCode, Intent data) {
            super.onActivityResult(activity, requestCode, resultCode, data);

            Uri selectedImageUri = data.getData();
            System.out.println("requestCode = " + requestCode);
            System.out.println("resultCode = " + resultCode);
            System.out.println("selectedImageUri = " + selectedImageUri);

            String realPathFromURI = resolve(selectedImageUri);
            System.out.println("realPathFromURI = " + realPathFromURI);

            if (requestCode == REQ_CODE_GALLERY_PICKER) {
                while (!callbacks.isEmpty()) {
                    Promise promise = callbacks.remove(0);
                    promise.resolve(realPathFromURI);
                }
            }
        }

        public void add(Promise promise) {
            callbacks.add(promise);
        }

        private String resolve(Uri uri) {
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
}