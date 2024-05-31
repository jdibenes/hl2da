
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
    public GameObject quad_pv;
    public GameObject tmp_mic;
    public GameObject tmp_si;
    public GameObject tmp_ee;

    private Texture2D tex_lf;
    private Texture2D tex_ll;
    private Texture2D tex_rf;
    private Texture2D tex_rr;
    private Texture2D tex_ht;
    private Texture2D tex_ht_ab;
    private Texture2D tex_lt;
    private Texture2D tex_lt_ab;
    private Texture2D tex_lt_sigma;
    private Texture2D tex_pv;

    // data vlc: u8 image, pose
    // data ahat: u16 z, u16 ab, pose
    // data lt: u16 z, u16 ab, u8 sigma, pose
    // data imu: AoS, pose
    // PV: u8 image, float intrinsics, pose
    // MC: float samples
    // SI: float head, float eye, S left, S right
    // EE: float eyedata, pose

    private Dictionary<hl2da_api.SENSOR_ID, int> last_framestamp = new Dictionary<hl2da_api.SENSOR_ID, int>();
    hl2da_api.pv_captureformat pvcf;

    // Start is called before the first frame update
    void Start()
    {
        hl2da_user.InitializeComponents();
        hl2da_user.OverrideWorldCoordinateSystem();

        
        float[,] extrinsics_lf  = hl2da_user.GetExtrinsics_RM(hl2da_api.SENSOR_ID.RM_VLC_LEFTFRONT);
        float[,] extrinsics_ll  = hl2da_user.GetExtrinsics_RM(hl2da_api.SENSOR_ID.RM_VLC_LEFTLEFT);
        float[,] extrinsics_rf  = hl2da_user.GetExtrinsics_RM(hl2da_api.SENSOR_ID.RM_VLC_RIGHTFRONT);
        float[,] extrinsics_rr  = hl2da_user.GetExtrinsics_RM(hl2da_api.SENSOR_ID.RM_VLC_RIGHTRIGHT);
        float[,] extrinsics_ht  = hl2da_user.GetExtrinsics_RM(hl2da_api.SENSOR_ID.RM_DEPTH_AHAT);
        float[,] extrinsics_lt  = hl2da_user.GetExtrinsics_RM(hl2da_api.SENSOR_ID.RM_DEPTH_LONGTHROW);
        float[,] extrinsics_acc = hl2da_user.GetExtrinsics_RM(hl2da_api.SENSOR_ID.RM_IMU_ACCELEROMETER);
        float[,] extrinsics_gyr = hl2da_user.GetExtrinsics_RM(hl2da_api.SENSOR_ID.RM_IMU_GYROSCOPE);

        
        float[,] image_points = new float[1, 2];
        float[,] camera_points = new float[1, 2];

        image_points[0, 0] = 320.0f;
        image_points[0, 1] = 240.0f;

        camera_points[0, 0] = 0.0f;
        camera_points[0, 1] = 0.0f;

        float[,] mpoint = hl2da_user.MapImagePointToCameraUnitPlane_RM(hl2da_api.SENSOR_ID.RM_VLC_LEFTFRONT, image_points);
        float[,] ppoint = hl2da_user.MapCameraSpaceToImagePoint_RM(hl2da_api.SENSOR_ID.RM_VLC_LEFTFRONT, camera_points);


        pvcf = hl2da_user.CreateFormat_PV(640, 360, 30, false, false);

        hl2da_user.BypassDepthLock_RM(true);
        hl2da_user.SetFormat_PV(pvcf);
        hl2da_user.SetFormat_Microphone(hl2da_api.MC_CHANNELS.USE_2);
        hl2da_user.SetFormat_ExtendedEyeTracking(hl2da_api.EE_FPS_INDEX.FPS_90);

        hl2da_user.Initialize(hl2da_api.SENSOR_ID.RM_VLC_LEFTFRONT,       30); // Limited by memory
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.RM_VLC_LEFTLEFT,        30); // Limited by memory
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.RM_VLC_RIGHTFRONT,      30); // Limited by memory
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.RM_VLC_RIGHTRIGHT,      30); // Limited by memory
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.RM_DEPTH_AHAT,          45); // Limited by memory
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.RM_DEPTH_LONGTHROW,     15); // Limited by internal buffer - Maximum is 18
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.RM_IMU_ACCELEROMETER,   12); // Limited by memory
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.RM_IMU_GYROSCOPE,       24); // Limited by memory
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.RM_IMU_MAGNETOMETER,     5); // Limited by memory
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.PV,                     15); // Limited by internal buffer - Maximum is 18
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.MICROPHONE,            125); // Limited by memory
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.SPATIAL_INPUT,          60); // Limited by memory
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.EXTENDED_EYE_TRACKING, 180); // Limited by memory

        hl2da_user.SetEnable(hl2da_api.SENSOR_ID.RM_VLC_LEFTFRONT,      true);
        hl2da_user.SetEnable(hl2da_api.SENSOR_ID.RM_VLC_LEFTLEFT,       true);
        hl2da_user.SetEnable(hl2da_api.SENSOR_ID.RM_VLC_RIGHTFRONT,     true);
        hl2da_user.SetEnable(hl2da_api.SENSOR_ID.RM_VLC_RIGHTRIGHT,     true);
        hl2da_user.SetEnable(hl2da_api.SENSOR_ID.RM_DEPTH_AHAT,         true);
        hl2da_user.SetEnable(hl2da_api.SENSOR_ID.RM_DEPTH_LONGTHROW,    true);
        hl2da_user.SetEnable(hl2da_api.SENSOR_ID.RM_IMU_ACCELEROMETER,  true);
        hl2da_user.SetEnable(hl2da_api.SENSOR_ID.RM_IMU_GYROSCOPE,      true);
        hl2da_user.SetEnable(hl2da_api.SENSOR_ID.RM_IMU_MAGNETOMETER,   true);
        hl2da_user.SetEnable(hl2da_api.SENSOR_ID.PV,                    true);
        hl2da_user.SetEnable(hl2da_api.SENSOR_ID.MICROPHONE,            true);
        hl2da_user.SetEnable(hl2da_api.SENSOR_ID.SPATIAL_INPUT,         true);
        hl2da_user.SetEnable(hl2da_api.SENSOR_ID.EXTENDED_EYE_TRACKING, true);

        tex_lf       = new Texture2D(640, 480, TextureFormat.R8,  false);
        tex_ll       = new Texture2D(640, 480, TextureFormat.R8,  false);
        tex_rf       = new Texture2D(640, 480, TextureFormat.R8,  false);
        tex_rr       = new Texture2D(640, 480, TextureFormat.R8,  false);
        tex_ht       = new Texture2D(512, 512, TextureFormat.R16, false);
        tex_ht_ab    = new Texture2D(512, 512, TextureFormat.R16, false);
        tex_lt       = new Texture2D(320, 288, TextureFormat.R16, false);
        tex_lt_ab    = new Texture2D(320, 288, TextureFormat.R16, false);
        tex_lt_sigma = new Texture2D(320, 288, TextureFormat.R8,  false);
        tex_pv       = new Texture2D(640, 360, TextureFormat.R8,  false);

        for (int i = 0; i < 13; ++i) { last_framestamp[(hl2da_api.SENSOR_ID)i] = -1; }
    }

    // Update is called once per frame
    void Update()
    {
        Update_RM_VLC(hl2da_api.SENSOR_ID.RM_VLC_LEFTFRONT,  tex_lf, quad_lf);
        Update_RM_VLC(hl2da_api.SENSOR_ID.RM_VLC_LEFTLEFT,   tex_ll, quad_ll);
        Update_RM_VLC(hl2da_api.SENSOR_ID.RM_VLC_RIGHTFRONT, tex_rf, quad_rf);
        Update_RM_VLC(hl2da_api.SENSOR_ID.RM_VLC_RIGHTRIGHT, tex_rr, quad_rr);
        Update_RM_Depth_AHAT();
        Update_RM_Depth_Longthrow();
        Update_RM_IMU_Accelerometer();
        Update_RM_IMU_Gyroscope();
        Update_RM_IMU_Magnetometer();
        Update_PV();
        Update_Microphone();
        Update_SpatialInput();
        Update_ExtendedEyeTracking();
    }

    bool IsNewFrame(hl2da_framebuffer fb)
    {
        if (fb.Framestamp <= last_framestamp[fb.Id]) { return false; }
        last_framestamp[fb.Id] = fb.Framestamp;
        return true;
    }

    void Update_RM_VLC(hl2da_api.SENSOR_ID id, Texture2D tex, GameObject quad)
    {
        // Get most recent frame
        hl2da_framebuffer fb = hl2da_framebuffer.GetFrame(id, -1);
        if ((fb.Status != hl2da_api.STATUS.OK) || !IsNewFrame(fb)) { return; } // Destroy if not new frame

        // Load frame data into textures
        tex.LoadRawTextureData(fb.Buffer(0), fb.Length(0) * sizeof(byte)); // Image is u8
        tex.Apply();

        float[,] pose = hl2da_user.Unpack2D<float>(fb.Buffer(3), hl2da_user.POSE_ROWS, hl2da_user.POSE_COLS);

        fb.Destroy();

        // Display frame
        quad.GetComponent<Renderer>().material.mainTexture = tex;
    }

    void Update_RM_Depth_AHAT()
    {
        // Get most recent frame
        hl2da_framebuffer fb = hl2da_framebuffer.GetFrame(hl2da_api.SENSOR_ID.RM_DEPTH_AHAT, -1);
        if ((fb.Status != hl2da_api.STATUS.OK) || !IsNewFrame(fb)) { return; }

        // Load frame data into textures
        tex_ht.LoadRawTextureData(fb.Buffer(0), fb.Length(0) * sizeof(ushort)); // Depth is u16
        tex_ht_ab.LoadRawTextureData(fb.Buffer(1), fb.Length(1) * sizeof(ushort)); // AB is u16
        tex_ht.Apply();
        tex_ht_ab.Apply();

        float[,] pose = hl2da_user.Unpack2D<float>(fb.Buffer(3), hl2da_user.POSE_ROWS, hl2da_user.POSE_COLS);

        fb.Destroy();

        // Display frame
        quad_ht.GetComponent<Renderer>().material.mainTexture = tex_ht;
        quad_ht_ab.GetComponent<Renderer>().material.mainTexture = tex_ht_ab;        
    }

    void Update_RM_Depth_Longthrow()
    {
        // Get most recent frame
        hl2da_framebuffer fb = hl2da_framebuffer.GetFrame(hl2da_api.SENSOR_ID.RM_DEPTH_LONGTHROW, -1);
        if ((fb.Status != hl2da_api.STATUS.OK) || !IsNewFrame(fb)) { return; }

        // Load frame data into textures
        tex_lt.LoadRawTextureData(fb.Buffer(0), fb.Length(0) * sizeof(ushort)); // Depth is u16
        tex_lt_ab.LoadRawTextureData(fb.Buffer(1), fb.Length(1) * sizeof(ushort)); // AB is u16
        tex_lt_sigma.LoadRawTextureData(fb.Buffer(2), fb.Length(2)); // Sigma is u8
        tex_lt.Apply();
        tex_lt_ab.Apply();
        tex_lt_sigma.Apply();

        float[,] pose = hl2da_user.Unpack2D<float>(fb.Buffer(3), hl2da_user.POSE_ROWS, hl2da_user.POSE_COLS);

        fb.Destroy();

        // Display frame
        quad_lt.GetComponent<Renderer>().material.mainTexture = tex_lt;
        quad_lt_ab.GetComponent<Renderer>().material.mainTexture = tex_lt_ab;
        quad_lt_sigma.GetComponent<Renderer>().material.mainTexture = tex_lt_sigma;
    }

    void Update_RM_IMU_Accelerometer()
    {
        // Get most recent frame
        hl2da_framebuffer fb = hl2da_framebuffer.GetFrame(hl2da_api.SENSOR_ID.RM_IMU_ACCELEROMETER, -1);
        if ((fb.Status != hl2da_api.STATUS.OK) || !IsNewFrame(fb)) { return; }

        // Unpack data
        hl2da_api.AccelDataStruct[] samples = hl2da_user.Unpack1D<hl2da_api.AccelDataStruct>(fb.Buffer(0), fb.Length(0));
        float[,] pose = hl2da_user.Unpack2D<float>(fb.Buffer(3), hl2da_user.POSE_ROWS, hl2da_user.POSE_COLS);

        // Display first sample in the batch
        hl2da_api.AccelDataStruct sample_0 = samples[0];
        tmp_acc.GetComponent<TextMeshPro>().text = string.Format("Accelerometer <{0}, {1}, {2}, {3}, {4}, {5}>", sample_0.VinylHupTicks, sample_0.SocTicks, sample_0.x, sample_0.y, sample_0.z, sample_0.temperature);
    }

    void Update_RM_IMU_Gyroscope()
    {
        // Get most recent frame
        hl2da_framebuffer fb = hl2da_framebuffer.GetFrame(hl2da_api.SENSOR_ID.RM_IMU_GYROSCOPE, -1);
        if ((fb.Status != hl2da_api.STATUS.OK) || !IsNewFrame(fb)) { return; }

        // Unpack data
        hl2da_api.GyroDataStruct[] samples = hl2da_user.Unpack1D<hl2da_api.GyroDataStruct>(fb.Buffer(0), fb.Length(0));
        float[,] pose = hl2da_user.Unpack2D<float>(fb.Buffer(3), hl2da_user.POSE_ROWS, hl2da_user.POSE_COLS);

        // Display first sample in the batch
        hl2da_api.GyroDataStruct sample_0 = samples[0];
        tmp_gyr.GetComponent<TextMeshPro>().text = string.Format("Gyroscope <{0}, {1}, {2}, {3}, {4}, {5}>", sample_0.VinylHupTicks, sample_0.SocTicks, sample_0.x, sample_0.y, sample_0.z, sample_0.temperature);
    }

    void Update_RM_IMU_Magnetometer()
    {
        // Get most recent frame
        hl2da_framebuffer fb = hl2da_framebuffer.GetFrame(hl2da_api.SENSOR_ID.RM_IMU_MAGNETOMETER, -1);
        if ((fb.Status != hl2da_api.STATUS.OK) || !IsNewFrame(fb)) { return; }

        // Unpack data
        hl2da_api.MagDataStruct[] samples = hl2da_user.Unpack1D<hl2da_api.MagDataStruct>(fb.Buffer(0), fb.Length(0));
        float[,] pose = hl2da_user.Unpack2D<float>(fb.Buffer(3), hl2da_user.POSE_ROWS, hl2da_user.POSE_COLS);

        // Display first sample in the batch
        hl2da_api.MagDataStruct sample_0 = samples[0];
        tmp_mag.GetComponent<TextMeshPro>().text = string.Format("Magnetometer <{0}, {1}, {2}, {3}, {4}>", sample_0.VinylHupTicks, sample_0.SocTicks, sample_0.x, sample_0.y, sample_0.z);
    }

    void Update_PV()
    {
        // Get most recent frame
        hl2da_framebuffer fb = hl2da_framebuffer.GetFrame(hl2da_api.SENSOR_ID.PV, -1);
        if ((fb.Status != hl2da_api.STATUS.OK) || !IsNewFrame(fb)) { return; }

        // Load frame data into textures
        tex_pv.LoadRawTextureData(fb.Buffer(0), pvcf.width * pvcf.height * sizeof(byte)); // Image is NV12, load Y only
        tex_pv.Apply();

        float[] intrinsics = hl2da_user.Unpack1D<float>(fb.Buffer(2), fb.Length(2)); // fx, fy, cx, cy
        float[,] pose = hl2da_user.Unpack2D<float>(fb.Buffer(3), hl2da_user.POSE_ROWS, hl2da_user.POSE_COLS);

        fb.Destroy();

        // Display frame
        quad_pv.GetComponent<Renderer>().material.mainTexture = tex_pv;
    }

    void Update_Microphone()
    {
        // Get most recent frame
        hl2da_framebuffer fb = hl2da_framebuffer.GetFrame(hl2da_api.SENSOR_ID.MICROPHONE, -1);
        if ((fb.Status != hl2da_api.STATUS.OK) || !IsNewFrame(fb)) { return; }

        // Display first five samples
        float[] samples = hl2da_user.Unpack1D<float>(fb.Buffer(0), fb.Length(0));

        tmp_mic.GetComponent<TextMeshPro>().text = string.Format("Microphone <{0}, {1}, {2}, {3}, {4}>", samples[0], samples[1], samples[2], samples[3], samples[4]);
    }

    void Update_SpatialInput()
    {
        // Get most recent frame
        hl2da_framebuffer fb = hl2da_framebuffer.GetFrame(hl2da_api.SENSOR_ID.SPATIAL_INPUT, -1);
        if ((fb.Status != hl2da_api.STATUS.OK) || !IsNewFrame(fb)) { return; }

        // Display head position
        float[] head_pose = hl2da_user.Unpack1D<float>(fb.Buffer(0), fb.Length(0));
        float[] eye_ray = hl2da_user.Unpack1D<float>(fb.Buffer(1), fb.Length(1));
        hl2da_api.JointPose[] left_hand = hl2da_user.Unpack1D<hl2da_api.JointPose>(fb.Buffer(2), fb.Length(2));
        hl2da_api.JointPose[] right_hand = hl2da_user.Unpack1D<hl2da_api.JointPose>(fb.Buffer(3), fb.Length(3));

        tmp_si.GetComponent<TextMeshPro>().text = string.Format("Spatial Input <{0}, {1}, {2}>", head_pose[0], head_pose[1], head_pose[2]);
    }

    void Update_ExtendedEyeTracking()
    {
        // Get most recent frame
        hl2da_framebuffer fb = hl2da_framebuffer.GetFrame(hl2da_api.SENSOR_ID.EXTENDED_EYE_TRACKING, -1);
        if ((fb.Status != hl2da_api.STATUS.OK) || !IsNewFrame(fb)) { return; }

        // Display combined gaze ray (origin, direction)
        float[] eye_data = hl2da_user.Unpack1D<float>(fb.Buffer(0), fb.Length(0));
        float[,] pose = hl2da_user.Unpack2D<float>(fb.Buffer(3), hl2da_user.POSE_ROWS, hl2da_user.POSE_COLS);

        tmp_ee.GetComponent<TextMeshPro>().text = string.Format("EE <{0}, {1}, {2}, {3}, {4}, {5}>", eye_data[0], eye_data[1], eye_data[2], eye_data[3], eye_data[4], eye_data[5]);
    }
}
