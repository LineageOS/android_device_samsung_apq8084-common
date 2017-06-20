/*
 * Copyright (C) 2017 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.cyanogenmod.hardware;

import org.cyanogenmod.internal.util.FileUtils;

import android.util.Log;

public class VibratorHW {

    private static final String TAG = "VibratorHW";
    private static final String LEVEL_PATH =
				"/sys/class/timed_output/vibrator/intensity";

    public static boolean isSupported() {
	boolean supported = FileUtils.isFileWritable(LEVEL_PATH) &&
			    FileUtils.isFileReadable(LEVEL_PATH);
	Log.v(TAG, "apq8084: supported = " + supported);
	return supported;
    }

    public static int getMaxIntensity() {
        return 10000;
    }

    public static int getMinIntensity() {
        return 0;
    }

    public static int getWarningThreshold() {
        return -1;
    }

    public static int getCurIntensity() {
        try {
	    /* LEVEL_PATH contains a string like "intensity: <integer>" */
	    String inp = FileUtils.readOneLine(LEVEL_PATH);
	    return Integer.parseInt(inp.replace("intensity: ",""));
        } catch (NumberFormatException e) {
            Log.e(TAG, e.getMessage(), e);
        }
        return -1;
    }

    public static int getDefaultIntensity() {
        return 10000;
    }

    public static boolean setIntensity(int intensity) {
        return FileUtils.writeLine(LEVEL_PATH, String.valueOf(intensity));
    }
}
