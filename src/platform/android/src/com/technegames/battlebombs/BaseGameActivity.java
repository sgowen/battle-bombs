package com.technegames.battlebombs;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.graphics.Point;
import android.media.AudioManager;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.Window;
import android.view.WindowManager;
import android.widget.RelativeLayout;
import android.widget.RelativeLayout.LayoutParams;

import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdView;

public abstract class BaseGameActivity extends Activity
{
    private static final Logger logger = new Logger(GameActivity.class);
    protected static final String EXTRA_USERNAME = "EXTRA_USERNAME";

    protected RendererWrapper _rendererWrapper;
    private GLSurfaceView _glSurfaceView;
    protected AdView _adView;
    protected String _username;

    protected abstract boolean isOffline();

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        // Do the stuff that initialize() would do for you
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FORCE_NOT_FULLSCREEN);

        _username = getIntent().getStringExtra(EXTRA_USERNAME);

        Point size = ViewUtils.getScreenSize(this);

        if (Logger.isDebugEnabled())
        {
            logger.debug("dimension " + size.x + " x " + size.y);
        }

        _rendererWrapper = new RendererWrapper(this, size.x, size.y, _username, isOffline());
        _glSurfaceView = new GLSurfaceView(this);
        RelativeLayout.LayoutParams glSurfaceViewLayoutParams = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.MATCH_PARENT);
        _glSurfaceView.setLayoutParams(glSurfaceViewLayoutParams);
        _glSurfaceView.setEGLContextClientVersion(2);
        _glSurfaceView.setRenderer(_rendererWrapper);

        setVolumeControlStream(AudioManager.STREAM_MUSIC);

        _glSurfaceView.setOnTouchListener(new OnTouchListener()
        {
            @SuppressLint("ClickableViewAccessibility")
            @Override
            public boolean onTouch(View v, final MotionEvent event)
            {
                synchronized (this)
                {
                    int action = event.getAction() & MotionEvent.ACTION_MASK;
                    int pointerIndex = (event.getAction() & MotionEvent.ACTION_POINTER_INDEX_MASK) >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;

                    switch (action)
                    {
                        case MotionEvent.ACTION_DOWN:
                        case MotionEvent.ACTION_POINTER_DOWN:
                            _rendererWrapper.handleTouchDown(event.getX(pointerIndex), event.getY(pointerIndex));
                            break;
                        case MotionEvent.ACTION_UP:
                        case MotionEvent.ACTION_POINTER_UP:
                        case MotionEvent.ACTION_CANCEL:
                            _rendererWrapper.handleTouchUp(event.getX(pointerIndex), event.getY(pointerIndex));
                            break;
                        case MotionEvent.ACTION_MOVE:
                            for (int i = 0; i < event.getPointerCount(); i++)
                            {
                                pointerIndex = i;
                                _rendererWrapper.handleTouchDragged(event.getX(pointerIndex), event.getY(pointerIndex));
                            }
                            break;
                    }

                    return true;
                }
            }
        });

        _adView = new AdView(this);
        _adView.setAdSize(AdSize.BANNER);
        _adView.setAdUnitId(getString(R.string.banner_ad_unit_id));
        RelativeLayout.LayoutParams adLayoutParams = new RelativeLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
        adLayoutParams.addRule(RelativeLayout.ALIGN_PARENT_TOP, RelativeLayout.TRUE);
        adLayoutParams.addRule(RelativeLayout.CENTER_HORIZONTAL, RelativeLayout.TRUE);
        _adView.setLayoutParams(adLayoutParams);

        RelativeLayout gameContainer = new RelativeLayout(this);
        RelativeLayout.LayoutParams gameContainerLayoutParams = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.MATCH_PARENT);
        gameContainer.setLayoutParams(gameContainerLayoutParams);

        gameContainer.addView(_glSurfaceView);
        gameContainer.addView(_adView);

        setContentView(gameContainer);

        AdRequest adRequest = new AdRequest.Builder().build();
        _adView.loadAd(adRequest);
    }

    @Override
    protected void onResume()
    {
        super.onResume();

        _glSurfaceView.onResume();
        _rendererWrapper.onResume();

        if (_adView != null)
        {
            _adView.resume();
        }
    }

    @Override
    protected void onPause()
    {
        _rendererWrapper.onPause();
        _glSurfaceView.onPause();

        if (_adView != null)
        {
            _adView.pause();
        }

        if (!isFinishing())
        {
            finish();
        }

        super.onPause();
    }

    @Override
    public void onDestroy()
    {
        if (_adView != null)
        {
            _adView.destroy();
        }

        super.onDestroy();
    }

    @Override
    public void onBackPressed()
    {
        if (_rendererWrapper.handleOnBackPressed())
        {
            return;
        }

        super.onBackPressed();
    }
}