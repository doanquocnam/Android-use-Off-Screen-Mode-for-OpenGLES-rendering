package com.namdq.glesoffscreenrender;

import android.content.pm.ActivityInfo;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.Window;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    static public int IMAGE_PREVIEW_WIDTH = 640;
    static public int IMAGE_PREVIEW_HEIGHT = 640;
    private int linearMainWidth;
    private int linearMainHeight;
    TextView textView;
    private LinearLayout linearMainLayout;
    public ImageView mainImageView;
    private Bitmap bitmap;
    static  {
        System.loadLibrary("gl2jni");
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        setContentView(R.layout.activity_main);
        linearMainLayout = (LinearLayout) findViewById(R.id.androidOpenGLLayout);
        linearMainLayout.post(new Runnable() {
            @Override
            public void run() {
                linearMainWidth = linearMainLayout.getWidth();
                linearMainHeight = linearMainLayout.getHeight();
                initWidget();
            }
        });
        Log.i(getClass().getName(), "onCreate: ");
    }

    public void initWidget()
    {
        int h;
        bitmap = Bitmap.createBitmap(IMAGE_PREVIEW_WIDTH, IMAGE_PREVIEW_HEIGHT, Bitmap.Config.ARGB_8888);
        mainImageView = (ImageView)findViewById(R.id.imageViewMain);//new ImageView(this);
        textView = (TextView)findViewById(R.id.txtFPS);

        double ratio = (double)linearMainHeight / linearMainWidth;
        h = linearMainWidth;
        if(ratio <= 1.5)
            h = linearMainWidth * 4 / 5;
        LinearLayout.LayoutParams param = new LinearLayout.LayoutParams(linearMainWidth, h);
        mainImageView.setLayoutParams(param);
        mainImageView.setBackgroundColor(Color.DKGRAY);
    }
    void onCallbackOpenGL()
    {
        //Log.i(getClass().getName(), "onCallbackOpenGL");
        mainImageView.invalidate();
        linearMainLayout.post(new Runnable() {
            @Override
            public void run() {
                mainImageView.setImageBitmap(bitmap);
            }
        });

    }
    void onCallbackFPS(final String strFPS)
    {
        textView.post(new Runnable() {
            @Override
            public void run() {
                textView.setText(strFPS);
            }
        });
    }
    @Override protected void onPause() {
        super.onPause();
        Log.i(getClass().getName(), "onPause()");
        pauseRender();
    }

    @Override protected void onResume() {
        super.onResume();
        Log.i(getClass().getName(), "onResume()");
        linearMainLayout.post(new Runnable() {
            @Override
            public void run() {
                startOrResumeRender(bitmap, getAssets());
            }
        });
    }
    @Override
    protected void onStop()
    {
        super.onStop();
        Log.i(getClass().getName(), "onStop()");

    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        Log.i(getClass().getName(), "onDestroy()");
    }
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void startOrResumeRender(Object bitmap, AssetManager asset);
    public native void stopRender();
    public native void pauseRender();
}
