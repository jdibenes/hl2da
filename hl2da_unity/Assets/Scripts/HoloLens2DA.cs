
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class HoloLens2DA : MonoBehaviour
{
    public GameObject quad_lf;
    public GameObject quad_ll;
    public GameObject quad_rf;
    public GameObject quad_rr;
    public GameObject quad_ht;
    public GameObject quad_ht_ab;
    public GameObject quad_lt;
    public GameObject quad_lt_ab;
    public GameObject quad_lt_sigma;
    public GameObject tmp_acc;
    public GameObject tmp_gyr;
    public GameObject tmp_mag;

    private Texture2D tex_lf;
    private Texture2D tex_ll;
    private Texture2D tex_rf;
    private Texture2D tex_rr;
    private Texture2D tex_ht;
    private Texture2D tex_ht_ab;
    private Texture2D tex_lt;
    private Texture2D tex_lt_ab;
    private Texture2D tex_lt_sigma;

    // Start is called before the first frame update
    void Start()
    {
        hl2da.InitializeComponents();
        hl2da.UpdateCoordinateSystem();

        
        float[,] extrinsics_lf  = hl2da.GetSensorExtrinsics(hl2da.sensor_id.RM_VLC_LEFTFRONT);
        float[,] extrinsics_ll  = hl2da.GetSensorExtrinsics(hl2da.sensor_id.RM_VLC_LEFTLEFT);
        float[,] extrinsics_rf  = hl2da.GetSensorExtrinsics(hl2da.sensor_id.RM_VLC_RIGHTFRONT);
        float[,] extrinsics_rr  = hl2da.GetSensorExtrinsics(hl2da.sensor_id.RM_VLC_RIGHTRIGHT);
        float[,] extrinsics_ht  = hl2da.GetSensorExtrinsics(hl2da.sensor_id.RM_DEPTH_AHAT);
        float[,] extrinsics_lt  = hl2da.GetSensorExtrinsics(hl2da.sensor_id.RM_DEPTH_LONGTHROW);
        float[,] extrinsics_acc = hl2da.GetSensorExtrinsics(hl2da.sensor_id.RM_IMU_ACCELEROMETER);
        float[,] extrinsics_gyr = hl2da.GetSensorExtrinsics(hl2da.sensor_id.RM_IMU_GYROSCOPE);

        
        float[,] image_points = new float[1, 2];
        float[,] camera_points = new float[1, 2];

        image_points[0, 0] = 320.0f;
        image_points[0, 1] = 240.0f;

        camera_points[0, 0] = 0.0f;
        camera_points[0, 1] = 0.0f;

        float[,] mpoint = hl2da.MapImagePointToCameraUnitPlane(hl2da.sensor_id.RM_VLC_LEFTFRONT, image_points);
        float[,] ppoint = hl2da.MapCameraSpaceToImagePoint(hl2da.sensor_id.RM_VLC_LEFTFRONT, camera_points);



        hl2da.InitializeStream(hl2da.sensor_id.RM_VLC_LEFTFRONT,     30);
        hl2da.InitializeStream(hl2da.sensor_id.RM_VLC_LEFTLEFT,      30);
        hl2da.InitializeStream(hl2da.sensor_id.RM_VLC_RIGHTFRONT,    30);
        hl2da.InitializeStream(hl2da.sensor_id.RM_VLC_RIGHTRIGHT,    30);
        hl2da.InitializeStream(hl2da.sensor_id.RM_DEPTH_AHAT,        45);
        hl2da.InitializeStream(hl2da.sensor_id.RM_DEPTH_LONGTHROW,    5); // must be small, don't hold too many frames for too long
        hl2da.InitializeStream(hl2da.sensor_id.RM_IMU_ACCELEROMETER, 12);
        hl2da.InitializeStream(hl2da.sensor_id.RM_IMU_GYROSCOPE,     24);
        hl2da.InitializeStream(hl2da.sensor_id.RM_IMU_MAGNETOMETER,   5);

        hl2da.SetStreamEnable(hl2da.sensor_id.RM_VLC_LEFTFRONT,     true);
        hl2da.SetStreamEnable(hl2da.sensor_id.RM_VLC_LEFTLEFT,      true);
        hl2da.SetStreamEnable(hl2da.sensor_id.RM_VLC_RIGHTFRONT,    true);
        hl2da.SetStreamEnable(hl2da.sensor_id.RM_VLC_RIGHTRIGHT,    true);
        hl2da.SetStreamEnable(hl2da.sensor_id.RM_DEPTH_AHAT,        true);
        hl2da.SetStreamEnable(hl2da.sensor_id.RM_DEPTH_LONGTHROW,   true);
        hl2da.SetStreamEnable(hl2da.sensor_id.RM_IMU_ACCELEROMETER, true);
        hl2da.SetStreamEnable(hl2da.sensor_id.RM_IMU_GYROSCOPE,     true);
        hl2da.SetStreamEnable(hl2da.sensor_id.RM_IMU_MAGNETOMETER,  true);

        tex_lf       = new Texture2D(640, 480, TextureFormat.R8,  false);
        tex_ll       = new Texture2D(640, 480, TextureFormat.R8,  false);
        tex_rf       = new Texture2D(640, 480, TextureFormat.R8,  false);
        tex_rr       = new Texture2D(640, 480, TextureFormat.R8,  false);
        tex_ht       = new Texture2D(512, 512, TextureFormat.R16, false);
        tex_ht_ab    = new Texture2D(512, 512, TextureFormat.R16, false);
        tex_lt       = new Texture2D(320, 288, TextureFormat.R16, false);
        tex_lt_ab    = new Texture2D(320, 288, TextureFormat.R16, false);
        tex_lt_sigma = new Texture2D(320, 288, TextureFormat.R8,  false);
    }

    // Update is called once per frame
    void Update()
    {
        Update_RM_VLC(hl2da.sensor_id.RM_VLC_LEFTFRONT,  tex_lf, quad_lf);
        Update_RM_VLC(hl2da.sensor_id.RM_VLC_LEFTLEFT,   tex_ll, quad_ll);
        Update_RM_VLC(hl2da.sensor_id.RM_VLC_RIGHTFRONT, tex_rf, quad_rf);
        Update_RM_VLC(hl2da.sensor_id.RM_VLC_RIGHTRIGHT, tex_rr, quad_rr);
        Update_RM_Depth_AHAT();
        Update_RM_Depth_Longthrow();
        Update_RM_IMU_Accelerometer();
        Update_RM_IMU_Gyroscope();
        Update_RM_IMU_Magnetometer();
    }

    void Update_RM_VLC(hl2da.sensor_id id, Texture2D tex, GameObject quad)
    {
        // Get most recent frame
        hl2da.frame_buffer fb = hl2da.GetStreamFrame(id, -1);
        if (fb.status != hl2da.get_status.OK) { return; }
        float[,] pose = hl2da.Unpack2D<float>(fb.pose_buffer, hl2da.POSE_ROWS, hl2da.POSE_COLS);

        // Load frame data into textures
        tex.LoadRawTextureData(fb.buffer, fb.length); // Image is u8
        tex.Apply();

        // Display frame
        quad.GetComponent<Renderer>().material.mainTexture = tex;
    }

    void Update_RM_Depth_AHAT()
    {
        // Get most recent frame
        hl2da.frame_buffer fb = hl2da.GetStreamFrame(hl2da.sensor_id.RM_DEPTH_AHAT, -1);
        if (fb.status != hl2da.get_status.OK) { return; }
        float[,] pose = hl2da.Unpack2D<float>(fb.pose_buffer, hl2da.POSE_ROWS, hl2da.POSE_COLS);

        // Load frame data into textures
        tex_ht.LoadRawTextureData(fb.buffer, fb.length * sizeof(ushort));                      // Depth is u16
        tex_ht_ab.LoadRawTextureData(fb.ab_depth_buffer, fb.ab_depth_length * sizeof(ushort)); // AB is u16
        tex_ht.Apply();
        tex_ht_ab.Apply();

        // Display frame
        quad_ht.GetComponent<Renderer>().material.mainTexture = tex_ht;
        quad_ht_ab.GetComponent<Renderer>().material.mainTexture = tex_ht_ab;        
    }

    void Update_RM_Depth_Longthrow()
    {
        // Get most recent frame
        hl2da.frame_buffer fb = hl2da.GetStreamFrame(hl2da.sensor_id.RM_DEPTH_LONGTHROW, -1);
        if (fb.status != hl2da.get_status.OK) { return; }
        float[,] pose = hl2da.Unpack2D<float>(fb.pose_buffer, hl2da.POSE_ROWS, hl2da.POSE_COLS);

        // Load frame data into textures
        tex_lt.LoadRawTextureData(fb.buffer, fb.length * sizeof(ushort));                      // Depth is u16
        tex_lt_ab.LoadRawTextureData(fb.ab_depth_buffer, fb.ab_depth_length * sizeof(ushort)); // AB is u16
        tex_lt_sigma.LoadRawTextureData(fb.sigma_buffer, fb.sigma_length);                     // Sigma is u8
        tex_lt.Apply();
        tex_lt_ab.Apply();
        tex_lt_sigma.Apply();

        // Display frame
        quad_lt.GetComponent<Renderer>().material.mainTexture = tex_lt;
        quad_lt_ab.GetComponent<Renderer>().material.mainTexture = tex_lt_ab;
        quad_lt_sigma.GetComponent<Renderer>().material.mainTexture = tex_lt_sigma;
    }

    void Update_RM_IMU_Accelerometer()
    {
        // Get most recent frame
        hl2da.frame_buffer fb = hl2da.GetStreamFrame(hl2da.sensor_id.RM_IMU_ACCELEROMETER, -1);
        if (fb.status != hl2da.get_status.OK) { return; }
        hl2da.AccelDataStruct[] samples = hl2da.Unpack1D<hl2da.AccelDataStruct>(fb.buffer, fb.length);
        float[,] pose = hl2da.Unpack2D<float>(fb.pose_buffer, hl2da.POSE_ROWS, hl2da.POSE_COLS);

        // Display first sample in the batch
        hl2da.AccelDataStruct sample_0 = samples[0];
        tmp_acc.GetComponent<TextMeshPro>().text = string.Format("Accelerometer <{0}, {1}, {2}, {3}, {4}, {5}>", sample_0.VinylHupTicks, sample_0.SocTicks, sample_0.x, sample_0.y, sample_0.z, sample_0.temperature);
    }

    void Update_RM_IMU_Gyroscope()
    {
        // Get most recent frame
        hl2da.frame_buffer fb = hl2da.GetStreamFrame(hl2da.sensor_id.RM_IMU_GYROSCOPE, -1);
        if (fb.status != hl2da.get_status.OK) { return; }
        hl2da.GyroDataStruct[] samples = hl2da.Unpack1D<hl2da.GyroDataStruct>(fb.buffer, fb.length);
        float[,] pose = hl2da.Unpack2D<float>(fb.pose_buffer, hl2da.POSE_ROWS, hl2da.POSE_COLS);

        // Display first sample in the batch
        hl2da.GyroDataStruct sample_0 = samples[0];
        tmp_gyr.GetComponent<TextMeshPro>().text = string.Format("Gyroscope <{0}, {1}, {2}, {3}, {4}, {5}>", sample_0.VinylHupTicks, sample_0.SocTicks, sample_0.x, sample_0.y, sample_0.z, sample_0.temperature);
    }

    void Update_RM_IMU_Magnetometer()
    {
        // Get most recent frame
        hl2da.frame_buffer fb = hl2da.GetStreamFrame(hl2da.sensor_id.RM_IMU_MAGNETOMETER, -1);
        if (fb.status != hl2da.get_status.OK) { return; }
        hl2da.MagDataStruct[] samples = hl2da.Unpack1D<hl2da.MagDataStruct>(fb.buffer, fb.length);
        float[,] pose = hl2da.Unpack2D<float>(fb.pose_buffer, hl2da.POSE_ROWS, hl2da.POSE_COLS);

        // Display first sample in the batch
        hl2da.MagDataStruct sample_0 = samples[0];
        tmp_mag.GetComponent<TextMeshPro>().text = string.Format("Magnetometer <{0}, {1}, {2}, {3}, {4}>", sample_0.VinylHupTicks, sample_0.SocTicks, sample_0.x, sample_0.y, sample_0.z);
    }
}
