/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.
Copyright (c) 2015 Adrian Dawid
 
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package org.cocos2dx.cpp;

//import org.cocos2dx.cpp.billing.BillingManager;
//import org.cocos2dx.cpp.billing.BillingProvider;
import org.cocos2dx.lib.Cocos2dxActivity;

import com.android.billingclient.api.BillingClient;
//import com.android.billingclient.api.SkuDetails;
import com.android.billingclient.api.SkuDetailsParams;
import com.android.billingclient.api.SkuDetailsResponseListener;
import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdLoader;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.InterstitialAd;
import com.google.android.gms.ads.formats.NativeAdOptions;
import com.google.android.gms.ads.formats.UnifiedNativeAd;
import com.google.android.gms.ads.reward.RewardItem;
import com.google.android.gms.ads.reward.RewardedVideoAd;
import com.google.android.gms.ads.reward.RewardedVideoAdListener;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.firebase.analytics.FirebaseAnalytics;
import kr.co.morongames.dd.*;
import kr.co.morongames.dd.R;

import android.annotation.TargetApi;
import android.content.IntentFilter;
import android.graphics.Color;
import android.graphics.Point;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceFragment;
import android.support.annotation.VisibleForTesting;
import android.view.Display;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.PopupWindow;
import android.widget.Toast;
import com.google.android.gms.common.api.ResultCallback;
import com.google.android.gms.games.Games;
import com.google.android.gms.games.Games.*;
import android.content.Context;
import android.util.Log;
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GooglePlayServicesUtil;
import com.google.android.gms.games.GamesStatusCodes;
import com.google.android.gms.games.leaderboard.LeaderboardVariant;
import com.google.android.gms.games.leaderboard.Leaderboards;

import android.content.Intent;
import android.app.Activity;
import android.util.Log;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.google.android.gms.ads.MobileAds;
import android.widget.LinearLayout;

import static com.android.billingclient.api.BillingClient.SkuType.INAPP;

import com.facebook.FacebookSdk;
import com.facebook.appevents.AppEventsLogger;


import com.iap.util.IabBroadcastReceiver;
import com.iap.util.IabHelper;
import com.iap.util.IabResult;
import com.iap.util.Inventory;
import com.iap.util.Purchase;
import com.iap.util.SkuDetails;
import com.unity3d.ads.UnityAds;
import com.unity3d.ads.IUnityAdsListener;

import com.facebook.FacebookSdk;

//public class AppActivity extends BaseGameActivity implements BillingProvider, RewardedVideoAdListener {
public class AppActivity extends BaseGameActivity implements IabBroadcastReceiver.IabBroadcastListener, RewardedVideoAdListener , IUnityAdsListener{


    // paid
//    static final String GOOGLE_SIGN = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAm6kiq5CGCWyOyrfikkR82ctsu1ygCc5frx8JFRT7AQuDxOzN+TNBew2kzk+U7r079FsdVqPJum5Ny4GtWn58kTPRUktP7gpg8U9aKKu0G5I6qdWYYIvzZvxz9Zk8vDTUvTQXkNt4tUHaRIf5yfv8LrJdKRszLMUNJQ7/dJuIcsQpYv65lbg+DsYMqguAvsXGccJIRxtGjxI/FRS0FitBKO41zYSRIY8YIvO6mLyOkBYQWSX6V2flfcvDMgJuIbrB9Bqnd6Lx707UgbHnoC4fUMeKDu76Aq0LsPdS0aVUsCZ0ZZqo36eBSTeZpKrRV6V1otz7d/O4jNoMbz4bKqXh6wIDAQAB";
    // free
    static final String GOOGLE_SIGN = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAkU4K+zHIJzBh9/lHYh0vcLHdvBDd5dmR+KmEu2SZX6mWamL1amtSN6Ai6uLU1qshbWupMxWYnpOlLEc8IMoPMPWde328t2dBQX+6UOcM95Ffsd7B/H2Px9TrpvIW7/Rb570PfIupUT2l/uv7Ozt5IQhziEG0v6vBuveWA2pycPWCzmId5TYhP/EY7cocFJ99viciCBMQPsS9BMeDWHxEsI7+YOalpGsc39G1AxJg5E31AYdRmG8L/Y2X/2xpjJsvcqYIQaqQpLnafPi0EJ+hBKc4BLFb6OASNRBvui1keeUFr0s4YTCKl+o2pGZFaKFuJCj6OBBaflIbRu4uVPDrLQIDAQAB";
    IabHelper mHelper = null;
    IabBroadcastReceiver mBroadcastReceiver;

    static final int RC_REQUEST = 10001;

    static int currentID;
    static int currentScore;
    static int currentAchievementID;
    static boolean gpgAvailable;
    static String TAG = "DailyDungeon";
    static String[] leaderboardIDs;
    static String[] achievementIDs;
    static Context currentContext;
    public static native void callCppCallback();
    List<String> skuList;
    List<String> nonConsumableSkuList;
//    private MainViewController mViewController;
//    private BillingManager mBillingManager;
    public static AppActivity mActivity;
    public static native void onIAPSuccess();
    public static native void onIAPFailed();
    public static native void onVideoDone();
    public static native void onConsumeFinished(String skuName);
    public static native void onRestored(String skuName);
//    public static native void onSignInPlayStore(String playID);

    public AppEventsLogger logger;
    private String PlayerID = "";
    public String StrArg0 = "";
    private static Handler handler;
    private final static int HANDLER_FB_LOG = 0;
    private final static int HANDLER_SETUP_IAB = 1;
    private final static int HANDLER_FIREBASE_LOG = 2;
//    private AdView mAdView = null;
    private boolean shouldBannerShow = false;
//    private InterstitialAd mInterstitialAd;
    private RewardedVideoAd mRewardedVideoAd;
    public boolean isRewardedVideoLoaded = false;

    private FirebaseAnalytics mFirebaseAnalytics;
    public String logID;
    public String logName;
    public String logDetail;
    Map<String, String> priceAmountMap = new HashMap<String, String>();
    Map<String, String> priceLocaleMap = new HashMap<String, String>();
    Map<String, String> priceCurrencyMap = new HashMap<String, String>();

    public String receiptSigndData;
    public String receiptSignature;
    public String IAP_Buy;
    public boolean IsConsumeForReward = false;
    @Override
    public void onSignInSucceeded(){
        gpgAvailable = true;
//        onSignInPlayStore(getPlayerID());
    }
    
    @Override
    public void onSignInFailed(){
        gpgAvailable = false;
    }
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        String leaderboardIdsRaw = getString(R.string.leaderboards);
        String achievementIdsRaw = getString(R.string.achievements);

        leaderboardIDs = leaderboardIdsRaw.split(";");
        achievementIDs = achievementIdsRaw.split(";");

        currentContext = this;

        super.onCreate(savedInstanceState);
        mActivity = this;
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        // Start the controller and load game data
        skuList = new ArrayList<> ();
        nonConsumableSkuList = new ArrayList<> ();

//        mViewController = new MainViewController(this);
        // Create and initialize BillingManager which talks to BillingLibrary
//        mBillingManager = new BillingManager(this, mViewController.getUpdateListener());
        logger = AppEventsLogger.newLogger(this);
        logger.logEvent("facebook starting dd");
//        paid
//        UnityAds.initialize(this, "3173772", this);
//        free
        UnityAds.initialize(this, "3369059", this);

        handler = new Handler() {
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case HANDLER_FB_LOG:
                        logFBHandler();
                        break;
                    case HANDLER_SETUP_IAB:
                        setupIABHandler();
                        break;
                    case HANDLER_FIREBASE_LOG:
                        firebaseLogHandler();
                        break;

                }
            }
        };


        // admob
        // release
        // paid
//        MobileAds.initialize(this, "ca-app-pub-9963713380469621~9081134824");
        // free -
        MobileAds.initialize(this, "ca-app-pub-9963713380469621~3411186655");
//        ** change ads unit id below in the request

        int width = getDisplaySize(getWindowManager().getDefaultDisplay()).x;
        int height = getDisplaySize(getWindowManager().getDefaultDisplay()).y;
        LinearLayout.LayoutParams adParams = new LinearLayout.LayoutParams(
                width, height);

        loadBannerFunc();
        mRewardedVideoAd = MobileAds.getRewardedVideoAdInstance(this);
        mRewardedVideoAd.setRewardedVideoAdListener(this);
        loadRewardedVideoAd();
//        mInterstitialAd = new InterstitialAd(this);
//        mInterstitialAd.setAdUnitId("ca-app-pub-7893694248975700/6251817812");
//        loadInterstitial();
//        mInterstitialAd.setAdListener(new AdListener() {
//            @Override
//            public void onAdLoaded() {
//                // Code to be executed when an ad finishes loading.
//            }
//
//            @Override
//            public void onAdFailedToLoad(int errorCode) {
//                // Code to be executed when an ad request fails.
//            }
//
//            @Override
//            public void onAdOpened() {
//                // Code to be executed when the ad is displayed.
//
//            }
//
//            @Override
//            public void onAdLeftApplication() {
//                // Code to be executed when the user has left the app.
//                if (!mActivity.mInterstitialAd.isLoaded()) {
//                    mActivity.loadInterstitial();
//                }
//            }
//
//            @Override
//            public void onAdClosed() {
//                // Code to be executed when the interstitial ad is closed.
//                if (!mActivity.mInterstitialAd.isLoaded()) {
//                    mActivity.loadInterstitial();
//                }
//            }
//        });
        // end admob

        // firebase
        mFirebaseAnalytics = FirebaseAnalytics.getInstance(this);

        // iab
//        setupIABHandler(); // it is excuted by cpp after put sku

        Log.e(TAG, "onCreate done" );
        // onCreate done init done
    }
    public void queryInventory(){
        try {

            Log.e(TAG, "sku query start" );
            mHelper.queryInventoryAsync(true, skuList,null, mGotInventoryListener);

            //mHelper.queryInventoryAsync(mGotInventoryListener);
        } catch (IabHelper.IabAsyncInProgressException e) {
            //   complain("Error querying inventory. Another async operation in progress.");
        }
    }

    //////////////////////////////////////////////구글 인앱 파트////////////////////////////////////////////
    public void MakeGoogleIapList()
    {
        /*
        try {
            if (mHelper != null) {
                mHelper.startSetup(new IabHelper.OnIabSetupFinishedListener() {
                    public void onIabSetupFinished(IabResult result) {

                        if (!result.isSuccess()) {
                            // Oh noes, there was a problem.
                            //complain("Problem setting up in-app billing: " + result);
                            //  Toast.makeText(AppActivity.this, "INAPP OK1", Toast.LENGTH_LONG).show();
                            return;
                        }

                        // Have we been disposed of in the meantime? If so, quit.
                        if (mHelper == null) return;





                        try {
                            mHelper.queryInventoryAsync(true,SkuList,mGotInventoryListener);


                        } catch (Exception e) {

                        }



                    }
                });
            }

        } catch (Exception e) {


        }
*/

    }



    @Override
    public void receivedBroadcast() {
        // Received a broadcast notification that the inventory of items has changed
        //  Log.d(TAG, "Received broadcast notification. Querying inventory.");
        try {
            mHelper.queryInventoryAsync(mGotInventoryListener);
        } catch (IabHelper.IabAsyncInProgressException e) {
            // complain("Error querying inventory. Another async operation in progress.");
        }
    }



    // Listener that's called when we finish querying the items and subscriptions we own
    IabHelper.QueryInventoryFinishedListener mGotInventoryListener = new IabHelper.QueryInventoryFinishedListener() {
        public void onQueryInventoryFinished(IabResult result, Inventory inventory) {
            Log.e(TAG, "sku query receive" );
            // Have we been disposed of in the meantime? If so, quit.
            if (mHelper == null) return;

            // Is it a failure?
            if (result.isFailure()) {
                return;
            }
   /*
            String iap_str = "10";
            Purchase premiumPurchase = inventory.getPurchase("norizabob.removead");

            //여기서 리스토어 처리하면 될 듯 한데...
            if (premiumPurchase != null && verifyDeveloperPayload(premiumPurchase)) {
                // Google_Restore_Ok = 1;

              //  Toast.makeText(AppActivity.this, "INAPP OK", Toast.LENGTH_SHORT).show();
            }


            Bundle querySkus = new Bundle();
            querySkus.putStringArrayList("ITEM_ID_LIST", (ArrayList<String>) skuList);
*/
            try {
                Log.e(TAG, "sku query start +" + inventory.toString());

//                List<SkuDetails> skuDetailsList = new ArrayList<SkuDetails>();
//                for(int i = 0; i < skuDetailsList.size();i++){
                Object[] skuDetailsList = inventory.mSkuMap.values().toArray();
                Log.e(TAG, "sku detail callback " + skuDetailsList.length);
                for(int i = 0; i < skuDetailsList.length; i++){
                    SkuDetails skuDetail = (SkuDetails)skuDetailsList[i];
                    long amount = (skuDetail.getPriceAmountMicros()/1000);
                    priceAmountMap.put(skuDetail.getSku(), Long.toString(amount));
                    priceCurrencyMap.put(skuDetail.getSku(), skuDetail.getPriceCurrencyCode());
                    priceLocaleMap.put(skuDetail.getSku(), skuDetail.getPrice());
                    Log.e(TAG, "sku detail - sku: " + skuDetail.getSku() + " amount: " + Long.toString(amount) + " currency: " + skuDetail.getPriceCurrencyCode() + " /price: " + skuDetail.getPrice());
                }

                // consume owned item when it starts if they had it
                Object[] purchaseList = inventory.mPurchaseMap.values().toArray();
                Log.e(TAG, "Owned purchase list count: " + purchaseList.length + "(sku buyItem for filter)");
                for(int i = 0; i < skuDetailsList.length; i++) {
                    Purchase purchase= (Purchase) purchaseList[i];
                    boolean isNonConsumable = false;
                    for(int j = 0; j < nonConsumableSkuList.size(); j++){
                        Log.e(TAG, "what consumable sku " + nonConsumableSkuList.get(j) +"/"+ purchase.getSku());
                        if(nonConsumableSkuList.get(j).equals(purchase.getSku())){
                            isNonConsumable = true;
                            Log.e(TAG, "non consumable sku " + purchase.getSku());
                            break;
                        }else{
                            Log.e(TAG, "consumable sku " + purchase.getSku());
                        }
                    }
                    if(isNonConsumable){
                        AppActivity.onRestored(purchase.getSku());
                    }else{
                        mHelper.consumeAsync(purchase, mConsumeFinishedListener);
                    }
                }

//
//
//                SkuDetails skudetail = inventory.getSkuDetails("lol_v05000.plus");
//
//                if (skudetail == null) {
//                    iap_str = String.format("%s|%s#%s#%s#%s", iap_str,"lol_v05000.plus","speed","1","$4.99");
//                    iap_str = String.format("%s|%s#%s#%s#%s", iap_str,"lol_v05001.plus","power","1","$4.99");
//                    iap_str = String.format("%s|%s#%s#%s#%s", iap_str,"lol_v05002.plus","ad_boost","1","$4.99");
//
//                    iap_str = String.format("%s|%s#%s#%s#%s", iap_str,"lol_p03000.plus","package1","1","$2.99");
//                    iap_str = String.format("%s|%s#%s#%s#%s", iap_str,"lol_p30000.plus","package2","1","$29.99");
//                    iap_str = String.format("%s|%s#%s#%s#%s", iap_str,"lol_p100000.plus","package3","1","$99.99");
//
//                    iap_str = String.format("%s|%s#%s#%s#%s", iap_str,"lol_g05000.plus","gem","60","$4.99");
//                    iap_str = String.format("%s|%s#%s#%s#%s", iap_str,"lol_g10000.plus","gem","150","$9.99");
//                    iap_str = String.format("%s|%s#%s#%s#%s", iap_str,"lol_g30000.plus","gem","600","$29.99");
//                    iap_str = String.format("%s|%s#%s#%s#%s", iap_str,"lol_g100000.plus","gem","2200","$99.99");
//
//
//                } else {
//
//                    iap_str = String.format("%s|%s#%s#%s#%s", iap_str,"lol_v05000.plus","speed","1",inventory.getSkuDetails("lol_v05000.plus").getPrice());
//                    iap_str = String.format("%s|%s#%s#%s#%s", iap_str,"lol_v05001.plus","power","1",inventory.getSkuDetails("lol_v05001.plus").getPrice());
//                    iap_str = String.format("%s|%s#%s#%s#%s", iap_str,"lol_v05002.plus","ad_boost","1",inventory.getSkuDetails("lol_v05002.plus").getPrice());
//
//                    iap_str = String.format("%s|%s#%s#%s#%s", iap_str,"lol_p03000.plus","package1","1",inventory.getSkuDetails("lol_p03000.plus").getPrice());
//                    iap_str = String.format("%s|%s#%s#%s#%s", iap_str,"lol_p30000.plus","package2","1",inventory.getSkuDetails("lol_p30000.plus").getPrice());
//                    iap_str = String.format("%s|%s#%s#%s#%s", iap_str,"lol_p100000.plus","package3","1",inventory.getSkuDetails("lol_p100000.plus").getPrice());
//
//                    iap_str = String.format("%s|%s#%s#%s#%s", iap_str,"lol_g05000.plus","gem","60",inventory.getSkuDetails("lol_g05000.plus").getPrice());
//                    iap_str = String.format("%s|%s#%s#%s#%s", iap_str,"lol_g10000.plus","gem","150",inventory.getSkuDetails("lol_g10000.plus").getPrice());
//                    iap_str = String.format("%s|%s#%s#%s#%s", iap_str,"lol_g30000.plus","gem","600",inventory.getSkuDetails("lol_g30000.plus").getPrice());
//                    iap_str = String.format("%s|%s#%s#%s#%s", iap_str,"lol_g100000.plus","gem","2200",inventory.getSkuDetails("lol_g100000.plus").getPrice());
//
//
//                }
//
//
            } catch (Exception e) {

            }



            //Toast.makeText(AppActivity.this, iap_str, Toast.LENGTH_SHORT).show();

            /*
             * Check for items we own. Notice that for each purchase, we check
             * the developer payload to see if it's correct! See
             * verifyDeveloperPayload().
             */

//            IAP_Info = iap_str;
//            setting_ok = true;


        }
    };
    IabHelper.OnIabPurchaseFinishedListener mPurchaseFinishedListener = new IabHelper.OnIabPurchaseFinishedListener() {
        public void onIabPurchaseFinished(IabResult result, Purchase purchase) {
            if (result.isFailure()) {
                if (purchase != null && verifyDeveloperPayload(purchase)) {
                    IAP_Buy = "BUY_FAIL";
                    onIAPFailed();
                    /*
                    if (purchase.getSku().equals("norizabob.removead")) {
                        //여기서 리스토어 처리하면 될 듯 한데...
                        IAP_Buy = "BUY_DONE";
                     //   Toast.makeText(AppActivity.this, "INAPP OK", Toast.LENGTH_SHORT).show();
                    } else {
                        IAP_Buy = "BUY_FAIL";
                    }
                    */
                } else {
                    IAP_Buy = "BUY_FAIL";
                    onIAPFailed();
                }
                //   Toast.makeText(AppActivity.this, result.getMessage(), Toast.LENGTH_SHORT).show();
                return;
            }

            if (purchase != null && verifyDeveloperPayload(purchase)) {
                //  if (purchase.getSku().equals("norizabob.removead")) {
                //여기서 리스토어 처리하면 될 듯 한데...
                //      IAP_Buy = "BUY_DONE";
                //   } else {
                try {
                    receiptSigndData = purchase.getOriginalJson();
                    receiptSignature = purchase.getSignature();
//                    mHelper.consumeAsync(purchase, mConsumeFinishedListener);
                    onIAPSuccess();
                } catch (Exception e) {
                    IAP_Buy = "BUY_FAIL";
                    onIAPFailed();
                }
                //    }
            } else {
                //  IAP_Buy = "BUY_FAIL";
                onIAPFailed();
            }
        }
    };

    // Called when consumption is complete
    IabHelper.OnConsumeFinishedListener mConsumeFinishedListener = new IabHelper.OnConsumeFinishedListener() {
        public void onConsumeFinished(Purchase purchase, IabResult result) {
            // System.out.println("Consumption finished. Purchase: " + purchase + ", result: " + result);

            // We know this is the "gas" sku because it's the only one we consume,
            // so we don't check which sku was consumed. If you have more than one
            // sku, you probably should check...

//            hideLoading();

            //  Toast.makeText(AppActivity.this,"INAPP4 consume" , Toast.LENGTH_LONG).show();
            if (result.isSuccess()) {
                //Iap_Sucess_Doit("");

//                receiptSigndData = purchase.getOriginalJson();
//                receiptSignature = purchase.getSignature();
                Log.e(TAG, "sku BUY done  consume finished: " + receiptSigndData);
                if(mActivity.IsConsumeForReward){
                    AppActivity.onConsumeFinished(purchase.getSku());
                    Log.e(TAG, "sku BUY done");
                }else{
                    Log.e(TAG, "sku BUY done2");
                }

//                if (verifyDeveloperPayload(purchase)) {
//
//                    // IAP_Buy = "BUY_DONE";
//
//                    IAP_Buy = purchase.getOriginalJson();
//
//                } else {
//                    IAP_Buy = "BUY_FAIL";
//                }
            }
            else {

                //	ToastMsg("FailPurchase2 " + result.getMessage());

                IAP_Buy = "BUY_FAIL";
                onIAPFailed();

            }

        }
    };

    /** Verifies the developer payload of a purchase. */
    boolean verifyDeveloperPayload(Purchase p) {
        String payload = p.getDeveloperPayload();


        /*
         * TODO: verify that the developer payload of the purchase is correct. It will be
         * the same one that you sent when initiating the purchase.
         *
         * WARNING: Locally generating a random string when starting a purchase and
         * verifying it here might seem like a good approach, but this will fail in the
         * case where the user purchases an item on one device and then uses your app on
         * a different device, because on the other device you will not have access to the
         * random string you originally generated.
         *
         * So a good developer payload has these characteristics:
         *
         * 1. If two different users purchase an item, the payload is different between them,
         *    so that one user's purchase can't be replayed to another user.
         *
         * 2. The payload must be such that you can verify it even when the app wasn't the
         *    one who initiated the purchase flow (so that items purchased by the user on
         *    one device work on other devices owned by the user).
         *
         * Using your own server to store and verify developer payloads across app
         * installations is recommended.
         */


        return payload != null && now_payload.equals(payload);


        //  return true;
    }






    //과금
    public void IAP_Setting() {


//        if (ENABLE_INAPP == false) {
//            return;
//
//        }
//
//        if (setting_ok == false) {
//
//
//            if (BINARYTYPE.value() == BINARY_TYPE.GOOGLE_PLAY.value()) {
//
//                this.MakeGoogleIapList();
//
//            } else  if (BINARYTYPE.value() == BINARY_TYPE.ONE_STORE.value()) {
//
//                this.MakeOneStoreIapList();
//            }
//
//        }


    }


    public String IAP_STORE_INFO() {



//        if (setting_ok == true) {
//            return IAP_Info;
//        } else {
            return "nothing_data";
//        }
    }


    public void Buy_Item_Setting()
    {
        IAP_Buy = "nothing_data";
//        purchase_busy = false;


    }


    //if ("0910019307".equals(Tstore_iap_id))


    String now_payload = "";
    String iap_item_id = "";
    boolean purchase_busy = false;
    public String IAP_BUY_ITEM(String item_id, String item_type, String payload) {

        iap_item_id = item_id;

//        if (purchase_busy == false) {


//            purchase_busy = true;



//            if (BINARYTYPE.value() == BINARY_TYPE.GOOGLE_PLAY.value()) {

                if (mHelper != null) {

//                    if ("Restore".equals(item_type) ) {
//                        this.MakeGoogleIapList();
//                    } else {




                        /*
                            Date nowDate = new Date();

                            SimpleDateFormat formatter = new SimpleDateFormat("HHmmss");
                            int now_time = Integer.parseInt(formatter.format(nowDate));


                            now_payload = UserId + String.valueOf(now_time);
                        */

                        now_payload = payload;

                        try {

                            mHelper.launchPurchaseFlow(this, item_id, RC_REQUEST,
                                    mPurchaseFinishedListener, now_payload);
                        } catch (Exception e) {

                            IAP_Buy = "BUY_FAIL";
                            onIAPFailed();
                        }



//                    }

                }

//            } else if (BINARYTYPE.value() == BINARY_TYPE.CURTURELAND.value()) {


/*
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            // Auto Loing Check


                            TelephonyManager telManager = (TelephonyManager)getSystemService(Context.TELEPHONY_SERVICE);
                            String phoneNum = telManager.getLine1Number();
                            // 결에 보낼 파라메타 값
                            if(phoneNum == null){
                                phoneNum = "010-0000-0000";
                            }
                            else if( phoneNum.contains("+82")){
                                String _str = "0"+phoneNum.substring(3, phoneNum.length());
                                phoneNum = _str;
                            }


                            Date nowDate = new Date();

                            SimpleDateFormat formatter = new SimpleDateFormat("yyyyMMddHHmmss");


                            Random generator = new Random();
                            now_payload = "COINPRINCESS" + formatter.format(nowDate) + generator.nextInt();


                            Log.d(" phoneNum ",   phoneNum);
                            payment.Login_to_pay(CurturelandPayKey, "null", iap_item_id,
                                    CurturelandAppCode, "P", phoneNum, now_payload,
                                    "com.daerisoft.tajjaculture");
                        } catch (Exception e) {
                            // TODO Auto-generated catch block
                            e.printStackTrace();

                            Toast.makeText(myActivity,e.getMessage(), Toast.LENGTH_LONG).show();

                            if (purchase_busy == true) {
                                IAP_Buy = "BUY_FAIL";
                            }

                        }
                    }
                });

                */


//            } else if (BINARYTYPE.value() == BINARY_TYPE.ONE_STORE.value()) {
//
//
//
//                if ("Restore".equals(iap_item_id) && OneStore_Restore == true) {
//
//
//                    runOnUiThread(new Runnable() {
//
//                        @Override
//                        public void run() {
//
//
//
//                            if (purchase_busy == true) {
//
//                                IAP_Buy = "BUY_DONE";
//                            }
//
//                            Toast.makeText(myActivity, "이미 구매한 항목입니다! 반영해드렸습니다!", Toast.LENGTH_LONG).show();
//
//
//                        }
//
//                    });
//
//
//
//                } else {
//
//
//                    runOnUiThread(new Runnable() {
//
//                        @Override
//                        public void run() {
//
//
//
//                            ParamsBuilder pb = new ParamsBuilder();
//
//                            pb.put(ParamsBuilder.KEY_APPID,
//                                    ONESTORE_ID).put(
//                                    ParamsBuilder.KEY_PID, iap_item_id.trim());
//
//                            String parameter = pb.build();
//
//
//                            Bundle req = mPlugin.sendPaymentRequest(parameter,
//                                    new IapPlugin.RequestCallback() {
//
//                                        @Override
//                                        public void onResponse(IapResponse data) {
//                                            if (data == null || data.getContentLength() <= 0) {
//                                                // TODO Unusual error
//                                                if (purchase_busy == true) {
//                                                    IAP_Buy = "BUY_FAIL";
//                                                }
//
//                                                return;
//                                            }
//
//                                            org.cocos2dx.cpp.pdu.Response response = ConverterFactory.getConverter().fromJson(
//                                                    data.getContentToString());
//
//                                            if (response == null) {
//                                                // TODO invalid response data
//                                                if (purchase_busy == true) {
//                                                    IAP_Buy = "BUY_FAIL";
//                                                }
//
//                                                return;
//                                            }
//
//                                            // TODO for logging
//                                            StringBuffer sb = new StringBuffer("onResponse() \n");
//                                            sb.append("From:" + data.getContentToString())
//                                                    .append("\n")
//                                                    .append("To:" + response.toString());
//
//                                            // response.result.code
//                                            if (!response.result.code.equals("0000")) {
//                                                if (purchase_busy == true) {
//                                                    IAP_Buy = "BUY_FAIL";
//                                                }
//
//                                                return;
//                                            }
//
//                                            JSONObject json = new JSONObject();
//                                            try {
//                                                json.put("appid", ONESTORE_ID);
//                                                json.put("txid", response.result.txid);
//                                                Log.d("Receipt", response.result.receipt.length() + " is the receipt ");
//                                                json.put("signdata", response.result.receipt);
//                                            } catch (JSONException e) {
//                                                e.printStackTrace();
//                                                // PaymentFragment.this.mLog
//                                                //    .setText("Failed while composing json data for verification receipt.");
//                                            }
//
//                                            ReceiptConfirm rc = new ReceiptConfirm();
//                                            rc.execute((String) json.toString());
//
//
//                                        }
//
//                                        @Override
//                                        public void onError(String reqid, String errcode,
//                                                            String errmsg) {
//                                            // TODO Error occurred
//
//
//                                            if (purchase_busy == true) {
//                                                IAP_Buy = "BUY_FAIL";
//                                            }
//                                        }
//                                    });
//
//
//
//
//
//                        }
//
//
//
//                    });
//
//                }
//
//
//
//            }
//
//
//        }


        //System.out.println("RES 4" + iap_item_id +" " + Tstore_Restore);

        // System.out.println("IAP_TEST " + IAP_Buy);


        return IAP_Buy;

    }



    public static String StringXorCalculater(String s1,String s2){
        StringBuilder sb = new StringBuilder();
        for(int i=0; i<s1.length() && i<s2.length();i++){
            sb.append((char)(s1.charAt(i) ^ s2.charAt(i)));
        }
        return sb.toString();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





    // admob
    private void loadRewardedVideoAd() {
        mActivity.runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                AdRequest adRequest = new AdRequest.Builder()
//                        .addTestDevice("182B7F0E6868A0AC0A17FA28CF9D312D") // g7
                        .build();
//                mRewardedVideoAd.loadAd("ca-app-pub-3940256099942544/5224354917",adRequest); // test
//                paid
                mRewardedVideoAd.loadAd("ca-app-pub-9963713380469621/4419124642",adRequest); // release
//                free
                mRewardedVideoAd.loadAd("ca-app-pub-9963713380469621/4341124945",adRequest); // release
            }
        });
    }

    @Override
    public void onRewarded(RewardItem reward) {
//        Toast.makeText(this, "onRewarded! currency: " + reward.getType() + "  amount: " + reward.getAmount(), Toast.LENGTH_SHORT).show();
        // Reward the user.
    }

    @Override
    public void onRewardedVideoAdLeftApplication() {
//        Toast.makeText(this, "onRewardedVideoAdLeftApplication", Toast.LENGTH_SHORT).show();
    }

    @Override
    public void onRewardedVideoAdClosed() {
//        Toast.makeText(this, "onRewardedVideoAdClosed", Toast.LENGTH_SHORT).show();
    }

    @Override
    public void onRewardedVideoAdFailedToLoad(int errorCode) {
//        Toast.makeText(this, "onRewardedVideoAdFailedToLoad", Toast.LENGTH_SHORT).show();
        Log.e(TAG, "admob video onRewardedVideoAdFailedToLoad " + errorCode);
        mActivity.isRewardedVideoLoaded = false;
    }

    @Override
    public void onRewardedVideoAdLoaded() {
//        Toast.makeText(this, "onRewardedVideoAdLoaded", Toast.LENGTH_SHORT).show();
        Log.e(TAG, "admob video onRewardedVideoAdLoaded");
        mActivity.isRewardedVideoLoaded = true;
    }

    @Override
    public void onRewardedVideoAdOpened() {
//        Toast.makeText(this, "onRewardedVideoAdOpened", Toast.LENGTH_SHORT).show();
    }

    @Override
    public void onRewardedVideoStarted() {
//        Toast.makeText(this, "onRewardedVideoStarted", Toast.LENGTH_SHORT).show();
    }

    @Override
    public void onRewardedVideoCompleted() {
//        Toast.makeText(this, "onRewardedVideoCompleted", Toast.LENGTH_SHORT).show();
        Log.e(TAG, "admob video onRewardedVideoCompleted");
        onVideoDone();
        loadRewardedVideoAd();
    }
    private void loadInterstitial(){
        AdRequest adRequest = new AdRequest.Builder()
//                .addTestDevice("182B7F0E6868A0AC0A17FA28CF9D312D")
                .build();
//        mInterstitialAd.loadAd(adRequest);
    }
    public static void setupIAB(){
        Message msg = new Message();
        msg.what = HANDLER_SETUP_IAB;
        handler.sendMessage(msg);
    }
    public void setupIABHandler(){
        String base64EncodedPublicKey = GOOGLE_SIGN;
//        mHelper = new IabHelper(this, Decode_data(base64EncodedPublicKey));
        mHelper = new IabHelper(this, base64EncodedPublicKey);
        mHelper.enableDebugLogging(false);
        mHelper.startSetup(new IabHelper.OnIabSetupFinishedListener() {
            public void onIabSetupFinished(IabResult result) {

                if (!result.isSuccess()) {
                    // Oh noes, there was a problem.
                    //complain("Problem setting up in-app billing: " + result);
                    //  Toast.makeText(AppActivity.this, "INAPP OK1", Toast.LENGTH_LONG).show();
                    return;
                }

                // Have we been disposed of in the meantime? If so, quit.
                if (mHelper == null) return;

                mBroadcastReceiver = new IabBroadcastReceiver(AppActivity.this);
                IntentFilter broadcastFilter = new IntentFilter(IabBroadcastReceiver.ACTION);
                registerReceiver(mBroadcastReceiver, broadcastFilter);

                // consume request when starting the app
                IsConsumeForReward = true;
                queryInventory();
            }
        });
    }
    // consume request after verifying receipt. also. You can use it for restore.
    public static void consumePurchased(){
        Log.e(TAG, "consumePurchased call");
        mActivity.runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                Log.e(TAG, "sku buyitem consume consumePurchased");
                // consume request after verifying receipt
                mActivity.IsConsumeForReward = false;
                mActivity.queryInventory();
            }
        });
    }
    public static void restoreIAB(){
        consumePurchased();;
    }
    public static void showInterstitial(){
        Log.e(TAG, "admob showInterstitial call");
//        mActivity.runOnUiThread(new Runnable()
//        {
//            @Override
//            public void run()
//            {
//                Log.e(TAG, "admob showInterstitial");
//                if (mActivity.mInterstitialAd.isLoaded()) {
//                    mActivity.mInterstitialAd.show();
//                } else {
//                    Log.e(TAG, "admob showInterstitial not loaded");
//                    Log.d("TAG", "The interstitial wasn't loaded yet.");
//                    mActivity.loadInterstitial();
//                }
//            }
//        });
    }
    public static void firebaseLog(String logID, String logName, String logDetail){
        Log.e(TAG, "firebase log");
        mActivity.logID = logID;
        mActivity.logName = logName;
        mActivity.logDetail = logDetail;

        Message msg = new Message();
        msg.what = HANDLER_FIREBASE_LOG;
        handler.sendMessage(msg);
//        mActivity.runOnUiThread(new Runnable()
//        {
//            @Override
//            public void run()
//            {
//                Log.e(TAG, "firebase log");
//                Bundle bundle = new Bundle();
//                bundle.putString(FirebaseAnalytics.Param.ITEM_ID, mActivity.logID);
//                bundle.putString(FirebaseAnalytics.Param.ITEM_NAME, mActivity.logName);
//                if(mActivity.logDetail.length() > 0) bundle.putString(FirebaseAnalytics.Param.CONTENT_TYPE, "image");
//                mActivity.mFirebaseAnalytics.logEvent(FirebaseAnalytishowRewardedVideoAdscs.Event.SELECT_CONTENT, bundle);
//            }
//        });
    }
    public void firebaseLogHandler(){
        Log.e(TAG, "firebase log handler");
        Bundle bundle = new Bundle();
        bundle.putString(FirebaseAnalytics.Param.ITEM_ID, mActivity.logID);
        bundle.putString(FirebaseAnalytics.Param.ITEM_NAME, mActivity.logName);
        if(mActivity.logDetail.length() > 0) bundle.putString(FirebaseAnalytics.Param.CONTENT_TYPE, mActivity.logDetail);
        mActivity.mFirebaseAnalytics.logEvent(FirebaseAnalytics.Event.SELECT_CONTENT, bundle);

    }
    public static void loadBanner(){
        mActivity.runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                mActivity.loadBannerFunc();
            }
        });
    }
    private void loadBannerFunc(){
        AdRequest adRequest = new AdRequest.Builder()
//                .addTestDevice("182B7F0E6868A0AC0A17FA28CF9D312D")
                .build();

//        mAdView.loadAd(adRequest);

        Log.e(TAG, "admob loadBanner");


    }
    public static void hideBanner()
    {
//        if(mActivity.mAdView == null) return;
//        mActivity.shouldBannerShow = false;
//        mActivity.runOnUiThread(new Runnable()
//        {
//            @Override
//            public void run()
//            {
//                Log.e(TAG, "admob hideBanner");
//                if (mActivity.mAdView.isEnabled())
//                    mActivity.mAdView.setEnabled(false);
//                if (mActivity.mAdView.getVisibility() != View.INVISIBLE  )
//                    mActivity.mAdView.setVisibility(View.INVISIBLE);
//            }
//        });
    }

    public static void showBanner()
    {
//        if(mActivity.mAdView == null) return;
//        mActivity.shouldBannerShow = true;
//        mActivity.runOnUiThread(new Runnable()
//        {
//            @Override
//            public void run()
//            {
//                Log.e(TAG, "admob showBanner");
//                if (!mActivity.mAdView.isEnabled())
//                    mActivity.mAdView.setEnabled(true);
//                if (mActivity.mAdView.getVisibility() == View.INVISIBLE ) {
//                    mActivity.mAdView.setVisibility(View.VISIBLE);
//                    mActivity.loadBannerFunc();
//                }
//
//            }
//        });
    }
    // Helper get display screen to avoid deprecated function use
    private Point getDisplaySize(Display d)
    {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB)
        {
            return getDisplaySizeGE11(d);
        }
        return getDisplaySizeLT11(d);
    }

    @TargetApi(Build.VERSION_CODES.HONEYCOMB_MR2)
    private Point getDisplaySizeGE11(Display d)
    {
        Point p = new Point(0, 0);
        d.getSize(p);
        return p;
    }
    private Point getDisplaySizeLT11(Display d)
    {
        try
        {
            Method getWidth = Display.class.getMethod("getWidth", new Class[] {});
            Method getHeight = Display.class.getMethod("getHeight", new Class[] {});
            return new Point(((Integer) getWidth.invoke(d, (Object[]) null)).intValue(), ((Integer) getHeight.invoke(d, (Object[]) null)).intValue());
        }
        catch (NoSuchMethodException e2) // None of these exceptions should ever occur.
        {
            return new Point(-1, -1);
        }
        catch (IllegalArgumentException e2)
        {
            return new Point(-2, -2);
        }
        catch (IllegalAccessException e2)
        {
            return new Point(-3, -3);
        }
        catch (InvocationTargetException e2)
        {
            return new Point(-4, -4);
        }
    }
    // admob ends

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // Check which request we're responding to
        if (mHelper == null) return;

        // 결과를 mHelper를 통해 처리합니다.
        if (!mHelper.handleActivityResult(requestCode, resultCode, data)) {
            // 처리할 결과물이 아닐경우 이곳으로 빠져 기본처리를 하도록 합니다.
            super.onActivityResult(requestCode, resultCode, data);

//            if (GAME_PLAY == true) {
//
//
//                if (resultCode == GamesActivityResultCodes.RESULT_RECONNECT_REQUIRED) {
//                    mHelper_gc.disconnect();
//                } else {
//
//                    mHelper_gc.onActivityResult(requestCode, resultCode, data);
//
//                }
//
//
//            }

        }

    }

    /**
     * This function assumes logger is an instance of AppEventsLogger and has been
     * created using AppEventsLogger.newLogger() call.
     */
//    public void logSentFriendRequestEvent () {
//        logger.logEvent("sentFriendRequest");
//    }


    public static void logFB(String msgToLog){
        Message msg = new Message();
        msg.what = HANDLER_FB_LOG;
        mActivity.StrArg0 = msgToLog;
        handler.sendMessage(msg);
    }
    private void logFBHandler(){
        logger.logEvent(StrArg0);
        Log.d("FB LOG", "log facebook " + StrArg0);

    }
    public static void showRewardedVideoAds() {
        if (mActivity.isRewardedVideoLoaded){
            mActivity.runOnUiThread(new Runnable()
            {
                @Override
                public void run()
                {
                    Log.e(TAG, "admob showRewardedVideoAds");
                    mActivity.mRewardedVideoAd.show();
                }
            });

        }else{
            Log.e(TAG, "admob not ready");
            if (UnityAds.isReady()) {
                Log.e(TAG, "showUnityAds ready");
                UnityAds.show(mActivity, "rewardedVideo");
            }else{
                Log.e(TAG, "showUnityAds not ready");
            }
        }
    }


    /* LISTENER */
    @Override
    public void onUnityAdsReady(final String zoneId) {
        Log.e(mActivity.TAG, "unityAdsReady: " + zoneId);
//            TextView statusText = (TextView) findViewById(R.id.unityads_example_statustext);
//            statusText.setText("");
//
//            DeviceLog.debug("onUnityAdsReady: " + zoneId);
//            Utilities.runOnUiThread(new Runnable() {
//                @Override
//                public void run() {
//                    // look for various default placement ids over time
//                    switch (zoneId) {
//                        case "video":
//                        case "defaultZone":
//                        case "defaultVideoAndPictureZone":
//                            interstitialPlacementId = zoneId;
//                            enableButton((Button) findViewById(R.id.unityads_example_interstitial_button));
//                            break;
//
//                        case "rewardedVideo":
//                        case "rewardedVideoZone":
//                        case "incentivizedZone":
//                            incentivizedPlacementId = zoneId;
//                            enableButton((Button) findViewById(R.id.unityads_example_incentivized_button));
//                            break;
//                    }
//                }
//            });
//
//            toast("Ready", zoneId);
    }

    @Override
    public void onUnityAdsStart(String zoneId) {
//            DeviceLog.debug("onUnityAdsStart: " + zoneId);
//            toast("Start", zoneId);
    }

    @Override
    public void onUnityAdsFinish(String zoneId, UnityAds.FinishState result) {
        Log.e(mActivity.TAG, "onUnityAdsFinish: " + zoneId);
        onVideoDone();
//            DeviceLog.debug("onUnityAdsFinish: " + zoneId + " - " + result);
//            toast("Finish", zoneId + " " + result);
    }

    @Override
    public void onUnityAdsError(UnityAds.UnityAdsError error, String message) {
//            DeviceLog.debug("onUnityAdsError: " + error + " - " + message);
//            toast("Error", error + " " + message);
//
//            TextView statusText = (TextView) findViewById(R.id.unityads_example_statustext);
//            statusText.setText(error + " - " + message);
    }
    private void toast(String callback, String msg) {
//            Toast.makeText(getApplicationContext(), callback + ": " + msg, Toast.LENGTH_SHORT).show();
    }
//     unityads listener ends
    public static void addSkuID(String skuID) {
        mActivity.skuList.add(skuID);
        Log.e(TAG, "addSkuID add " + skuID + " / total: "+mActivity.skuList.size());
//        skuList.add("goldenticket1");
//        skuList.add("premium_retry");
//        skuList.add("cc_starter1");
//        skuList.add("cc_starter3");
//        skuList.add("cc_starter10");
//        skuList.add("cc_gem3");
//        skuList.add("cc_gem10");
//        skuList.add("cc_gem50");
//        skuList.add("cc_gem100");
    }
    public static void addNonConsumableSkuID(String skuID) {
        mActivity.nonConsumableSkuList.add(skuID);
        Log.e(TAG, "nonConsumableSkuList add " + skuID + " / total: "+mActivity.nonConsumableSkuList.size());
    }
    public static void buyItem(String skuID){
//        ArrayList<String> currentSubscriptionSku = new ArrayList<>();
//        currentSubscriptionSku.add(skuID);
        try {
            String purchaseToken = "inapp_" + "daily_dungeon_" + skuID;
            Log.e("Cartoon Craft", "buyItem token: " + purchaseToken);
//            mActivity.getBillingManager().consumeAsync(purchaseToken); // test now
//        int response = mService.consumePurchase(3, "com.magmon.cartooncraft", purchaseToken);
//        if (response == 0){
//            Log.e(TAG, "Consume complete");
//        }

            mActivity.IAP_BUY_ITEM(skuID, "Buy", "studioNapPayload_" + mActivity.priceAmountMap.get(skuID) + "_" + mActivity.priceCurrencyMap.get(skuID));
            Log.e("Cartoon Craft", "buyItem: " + skuID);
//            mActivity.getBillingManager().initiatePurchaseFlow(skuID, INAPP);
            
        }catch (Exception e) {

        }
    }
    @Override
    protected void onResume() {
        super.onResume();
        // Note: We query purchases in onResume() to handle purchases completed while the activity
        // is inactive. For example, this can happen if the activity is destroyed during the
        // purchase flow. This ensures that when the activity is resumed it reflects the user's
        // current purchases.
        try
        {
//            if (mBillingManager != null
//                    && mBillingManager.getBillingClientResponseCode() == BillingClient.BillingResponse.OK) {
//                mBillingManager.queryPurchases();
//            }
        }
        catch(Exception e){

        }
    }

//    @Override
//    public BillingManager getBillingManager() {
//        return mBillingManager;
//    }

    @Override
    public void onDestroy() {
//        if (mBillingManager != null) {
//            mBillingManager.destroy();
//        }
        super.onDestroy();
    }

    void onBillingManagerSetupFinished() {

//        mBillingManager.querySkuDetailsAsync(INAPP, skuList, new SkuDetailsResponseListener() {
//            @Override
//            public void onSkuDetailsResponse(int responseCode,
//                                             List<SkuDetails> skuDetailsList) {
//                Log.e(TAG, "sku detail callback");
//                for(int i = 0; i < skuDetailsList.size();i++){
//                    long amount = (skuDetailsList.get(i).getPriceAmountMicros()/1000000);
//                    priceAmountMap.put(skuDetailsList.get(i).getSku(), Long.toString(amount));
//                    priceCurrencyMap.put(skuDetailsList.get(i).getSku(), skuDetailsList.get(i).getPriceCurrencyCode());
//                    priceLocaleMap.put(skuDetailsList.get(i).getSku(), skuDetailsList.get(i).getPrice());
//                    Log.e(TAG, "sku detail - sku: " + skuDetailsList.get(i).getSku() + " amount: " + Long.toString(amount) + " currency: " + skuDetailsList.get(i).getPriceCurrencyCode() + " /price: " + skuDetailsList.get(i).getPrice());
//                }
//            }
//        });
    }

//    @VisibleForTesting
//    public MainViewController getViewController() {
//        return mViewController;
//    }
    /*@brief Changes the actvie leaderboard
      @param The index of the leaderboard
    */
    static public void openLeaderboard(int leaderboardID){
         currentID = leaderboardID;
    }
    
    /*@brief This function opens the leaderboards ui for an leaderboard id*/
    static public void openLeaderboardUI(){
        if(gpgAvailable){
                ((AppActivity)currentContext).runOnUiThread(new Runnable() {
            public void run() {
                ((AppActivity)currentContext).startActivityForResult(Games.Leaderboards.getLeaderboardIntent(((AppActivity)currentContext).getGameHelper().getApiClient(), leaderboardIDs[currentID]),2);
            }
                });
        }
    }
    
    static public boolean isGPGSupported(){
//        Log.e("Cartton Craft", "gpgAcailable " + gpgAvailable);
        return gpgAvailable;
    }
    
    /*@brief Submits a score to the leaderboard that is currently actvie*/
    static public void submitScoreToLeaderboard(int score)
    {
        if(gpgAvailable){
        Games.Leaderboards.submitScore(((AppActivity)currentContext).getGameHelper().getApiClient(),leaderboardIDs[currentID],score);
        }
    }
    static public String getReceiptSignature()
    {
        Log.e("Cartoon Craft", "AppActivity getReceiptSignature: " + mActivity.receiptSignature);
        return mActivity.receiptSignature;
    }
    static public String getReceiptSignedData()
    {
        Log.e("Cartoon Craft", "AppActivity getReceiptSignedData: " + mActivity.receiptSigndData);
        return mActivity.receiptSigndData;
    }
    static public String getPriceAmount(String sku)
    {
        return mActivity.priceAmountMap.get(sku);
    }
    static public String getPriceCurrency(String sku)
    {
        return mActivity.priceCurrencyMap.get(sku);
    }
    static public String getPriceLocale(String sku)
    {
        Log.e("sku", "getPriceLocale");
        Log.e("sku", sku + ": " + mActivity.priceLocaleMap.get(sku));
        return mActivity.priceLocaleMap.get(sku);
    }
    static public String getPlayerID(){
//        Log.e("Cartoon Craft", "AppActivity getPlayerID");
        try
        {
            if(gpgAvailable){
//            Log.e("Cartoon Craft", "AppActivity getPlayerID about to");
                if(mActivity.PlayerID.length() > 0){
                    return mActivity.PlayerID;
                }else{
                    GoogleApiClient client = mActivity.getGameHelper().getApiClient();
                    if(client == null){
                        return "";
                    }else{
                        mActivity.PlayerID = Games.Players.getCurrentPlayerId(client);
                        return mActivity.PlayerID;
                    }

                }
            }
        }
        catch(Exception e){

        }
        return "";
    }
    static public boolean isVideoAvailable(){
        if(!mActivity.isRewardedVideoLoaded){
            mActivity.loadRewardedVideoAd();
        }
        return mActivity.isRewardedVideoLoaded || UnityAds.isReady();
    }
    static public String getPlayerName(){
        if(gpgAvailable){
            GoogleApiClient client = mActivity.getGameHelper().getApiClient();
            if(client == null) {
                return "Noname";
            }else{
                return Games.Players.getCurrentPlayer(client).getDisplayName();
            }
        }
        return "noName";
    }
    static public void requestScoreFromLeaderboard()
    {
        if(gpgAvailable){
            Games.Leaderboards.loadCurrentPlayerLeaderboardScore(((AppActivity)currentContext).getGameHelper().getApiClient(), leaderboardIDs[currentID], LeaderboardVariant.TIME_SPAN_ALL_TIME, LeaderboardVariant.COLLECTION_PUBLIC).setResultCallback(new ResultCallback<Leaderboards.LoadPlayerScoreResult>() {
                @Override
                public void onResult(final Leaderboards.LoadPlayerScoreResult scoreResult) {
                    if (scoreResult.getStatus().getStatusCode() == GamesStatusCodes.STATUS_OK) {
                        AppActivity.currentScore = (int)scoreResult.getScore().getRawScore();
                        AppActivity.callCppCallback();
                    }
                }
            });
        }
    }

    static public int collectScore()
    {
        return AppActivity.currentScore;
    }
    
     /*@brief Shows the achievements ui*/
    static public void showAchievements() {
        if(gpgAvailable){
        ((AppActivity)currentContext).runOnUiThread(new Runnable() {
            public void run() {
                ((AppActivity)currentContext).startActivityForResult(Games.Achievements.getAchievementsIntent(((AppActivity)currentContext).getGameHelper().getApiClient()), 5);
            }
        });
        }
    }
    
    /*@brief Changes the actvie Achievement
      @param The index of the achievement in the list*/
    static public void openAchievement(int achievementID){
        currentAchievementID = achievementID;
    }
    
    static public void updateAchievement(int percentage){
        if(gpgAvailable){
       Games.Achievements.unlock(((AppActivity)currentContext).getGameHelper().getApiClient(), achievementIDs[currentAchievementID]);
        }
    }
    
    static public void exitGame()
    {
        Intent intent = new Intent(currentContext, MainActivity.class);
        MainActivity.exiting=true;
        currentContext.startActivity(intent);
    }
}


