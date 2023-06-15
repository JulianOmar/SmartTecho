package com.example.smarttecho;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraManager;
import android.os.Build;
import android.os.Bundle;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

public class activity2 extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_activity2);


        // Obtener una instancia del sensor de aceleración y del sensor de campo magnético
        SensorManager sensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        Sensor accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        Sensor magneticField = sensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);

        // Crear un oyente para los sensores
        SensorEventListener sensorListener = new SensorEventListener() {
            float[] accelerometerValues = new float[3];
            float[] magneticFieldValues = new float[3];

            @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
            @Override
            public void onSensorChanged(SensorEvent event) {
                // Obtener los valores del sensor de aceleración y del sensor de campo magnético
                if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
                    accelerometerValues = event.values.clone();
                } else if (event.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD) {
                    magneticFieldValues = event.values.clone();
                }

                // Calcular la matriz de rotación y la orientación del dispositivo
                float[] rotationMatrix = new float[9];
                SensorManager.getRotationMatrix(rotationMatrix, null, accelerometerValues, magneticFieldValues);
                float[] orientationValues = new float[3];
                SensorManager.getOrientation(rotationMatrix, orientationValues);

                // Obtener la orientación del dispositivo en grados
                float azimuth = (float) Math.toDegrees(orientationValues[0]);
                float pitch = (float) Math.toDegrees(orientationValues[1]);
                float roll = (float) Math.toDegrees(orientationValues[2]);

                // Si el dispositivo está boca abajo
                CameraManager cameraManager = (CameraManager) getSystemService(Context.CAMERA_SERVICE);
                if (pitch < -45 && pitch > -135) {
                    // Encender el flash
                    try {
                        String cameraId = cameraManager.getCameraIdList()[0];
                        cameraManager.setTorchMode(cameraId, true);
                    } catch (CameraAccessException e) {
                        e.printStackTrace();
                    }
                } else {
                    // Apagar el flash
                    try {
                        String cameraId = cameraManager.getCameraIdList()[0];
                        cameraManager.setTorchMode(cameraId, false);
                    } catch (CameraAccessException e) {
                        e.printStackTrace();
                    }
                }
            }

            @Override
            public void onAccuracyChanged(Sensor sensor, int accuracy) {}
        };

// Registrar el oyente para los sensores
        sensorManager.registerListener(sensorListener, accelerometer, SensorManager.SENSOR_DELAY_NORMAL);
        sensorManager.registerListener(sensorListener, magneticField, SensorManager.SENSOR_DELAY_NORMAL);




    }


}