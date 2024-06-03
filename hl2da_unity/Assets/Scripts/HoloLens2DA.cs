
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class HoloLens2DA : MonoBehaviour
{
    public GameObject[] rm_vlc_images;
    public GameObject[] rm_depth_ahat_images;
    public GameObject[] rm_depth_longthrow_images;

    public GameObject[] poses;
    public GameObject[] calibrations;

    public GameObject acc_text;
    public GameObject gyr_text;
    public GameObject mag_text;
    public GameObject pv_image;
    public GameObject mic_text;
    public GameObject[] si_text;
    public GameObject[] ee_text;

    private Texture2D[] tex_vlc;
    private Texture2D tex_ht;
    private Texture2D tex_ht_ab;
    private Texture2D tex_lt;
    private Texture2D tex_lt_ab;
    private Texture2D tex_lt_sigma;
    private Texture2D tex_pv;

    private Dictionary<hl2da_api.SENSOR_ID, string> sensor_names;
    private Dictionary<hl2da_api.SENSOR_ID, int> last_framestamp;

    private hl2da_api.SENSOR_ID sync;
    private hl2da_api.pv_captureformat pvcf;
    private hl2da_api.MC_CHANNELS mcch;
    private hl2da_api.EE_FPS_INDEX eefi;

    // Start is called before the first frame update
    void Start()
    {
        // PV format
        // List of supported resolutions and framerates at https://github.com/jdibenes/hl2ss/blob/main/etc/pv_configurations.txt
        // enable_mrc: Enable Mixed Reality Capture (holograms in video)
        // shared: Enable Shared Mode (for when other apps or modules are already using the PV camera, resolution and framerate parameters are ignored)
        pvcf = hl2da_user.CreateFormat_PV(640, 360, 30, false, false);

        // Additional PV capture settings
        // To change the focus mode, temporal denoising, white balance, exposure, scene mode, iso speed, or backlight compensation,
        // after calling Initialize(PV, buffer_size) and SetEnable(PV, true),
        // wait to receive at least one frame (from hl2da_framebuffer.GetFrame with hl2da_api.STATUS.OK),
        // then these settings can be changed freely as long as the stream is enabled.
        // Unlike PV format, these settings are not latched internally, so they will be lost if the stream is restarted (via SetEnable(PV, false) -> SetEnable(PV, true))
        // and they must be reapplied using the SetEnable -> Wait 1 frame procedure.

        // Microphone format
        // hl2da_api.MC_CHANNELS.USE_2: Preprocessed 2-channel audio
        // hl2da_api.MC_CHANNELS.USE_5: Raw audio from the 5-channel HoloLens microphone array
        mcch = hl2da_api.MC_CHANNELS.USE_5;

        // Extended Eye Tracking framerate
        eefi = hl2da_api.EE_FPS_INDEX.FPS_90;

        // Synchronize to sensor (value not in hl2da_api.SENSOR_ID = no synchronization)
        sync = (hl2da_api.SENSOR_ID)(-1); // hl2da_api.SENSOR_ID.RM_DEPTH_LONGTHROW;
        
        Initialize_Dictionaries();

        hl2da_user.InitializeComponents();
        hl2da_user.OverrideWorldCoordinateSystem(); // Link Unity and plugin coordinate systems

        hl2da_user.SetFormat_PV(pvcf);
        hl2da_user.SetFormat_Microphone(mcch);
        hl2da_user.SetFormat_ExtendedEyeTracking(eefi);

        Update_Extrinsics();

        hl2da_user.BypassDepthLock_RM(true); // Allows simultaneous access to AHAT and longthrow depth

        // Use a buffer size of 2 seconds (except longthrow and PV)
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.RM_VLC_LEFTFRONT,       60); // Buffer size limited by memory                          // 30 Hz
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.RM_VLC_LEFTLEFT,        60); // Buffer size limited by memory                          // 30 Hz
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.RM_VLC_RIGHTFRONT,      60); // Buffer size limited by memory                          // 30 Hz
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.RM_VLC_RIGHTRIGHT,      60); // Buffer size limited by memory                          // 30 Hz
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.RM_DEPTH_AHAT,          90); // Buffer size limited by memory                          // 45 Hz
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.RM_DEPTH_LONGTHROW,     15); // Buffer size limited by internal buffer - Maximum is 18 // 5 Hz
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.RM_IMU_ACCELEROMETER,   24); // Buffer size limited by memory                          // ~12 Hz per batch
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.RM_IMU_GYROSCOPE,       48); // Buffer size limited by memory                          // ~24 Hz per batch
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.RM_IMU_MAGNETOMETER,    10); // Buffer size limited by memory                          // ~5 Hz per batch
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.PV,                     15); // Buffer size limited by internal buffer - Maximum is 18 // 5, 15, 30, 60 Hz selectable depending on chosen resolution 
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.MICROPHONE,            125); // Buffer size limited by memory                          // 768 samples per frame equivalent to 768/48000=16 ms of audio
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.SPATIAL_INPUT,          60); // Buffer size limited by memory                          // 30 Hz
        hl2da_user.Initialize(hl2da_api.SENSOR_ID.EXTENDED_EYE_TRACKING, 180); // Buffer size limited by memory                          // 30, 60, or 90 Hz selectable

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

        tex_vlc = new Texture2D[4];

        tex_vlc[0]   = new Texture2D(640, 480, TextureFormat.R8,  false);
        tex_vlc[1]   = new Texture2D(640, 480, TextureFormat.R8,  false);
        tex_vlc[2]   = new Texture2D(640, 480, TextureFormat.R8,  false);
        tex_vlc[3]   = new Texture2D(640, 480, TextureFormat.R8,  false);
        tex_ht       = new Texture2D(512, 512, TextureFormat.R16, false);
        tex_ht_ab    = new Texture2D(512, 512, TextureFormat.R16, false);
        tex_lt       = new Texture2D(320, 288, TextureFormat.R16, false);
        tex_lt_ab    = new Texture2D(320, 288, TextureFormat.R16, false);
        tex_lt_sigma = new Texture2D(320, 288, TextureFormat.R8,  false);

        tex_pv       = new Texture2D(pvcf.width, pvcf.height, TextureFormat.R8, false);
    }

    // Update is called once per frame
    void Update()
    {
        if ((sync < hl2da_api.SENSOR_ID.RM_VLC_LEFTFRONT) || (sync > hl2da_api.SENSOR_ID.EXTENDED_EYE_TRACKING))
        {
            Update_FFA();
        }
        else
        {
            Update_Sync();
        }
    }

    void Update_FFA()
    {
        for (hl2da_api.SENSOR_ID id = hl2da_api.SENSOR_ID.RM_VLC_LEFTFRONT; id <= hl2da_api.SENSOR_ID.EXTENDED_EYE_TRACKING; ++id)
        {
            // Get Frame
            // Negative index (-n): get nth most recent frame, may repeat or skip frames as necessary
            // Non-negative index (n>=0): get nth frame where 0 is the first frame ever since SetEnable(true), intended for sequential access (no skip or repeat)
            hl2da_framebuffer fb = hl2da_framebuffer.GetFrame(id, -1); // Get most recent frame

            // Check result
            // DISCARDED: requested frame is too old and has been removed from the internal buffer (cannot be recovered)
            // OK: got requested frame
            // WAIT: requested frame has not been captured yet (just wait, ideally 1/frame_rate seconds)
            if (fb.Status != hl2da_api.STATUS.OK) { continue; }

            Update_Sensor_Data(fb);
            fb.Destroy();
        }
    }

    void Update_Sync()
    {
        hl2da_framebuffer fb_ref = hl2da_framebuffer.GetFrame(sync, -2); // Use a small delay to allow receiving the optimal frame from the second stream
        if (fb_ref.Status != hl2da_api.STATUS.OK) { return; }
        ulong fb_ref_timestamp = fb_ref.Timestamp;
        Update_Sensor_Data(fb_ref);        
        fb_ref.Destroy();

        for (hl2da_api.SENSOR_ID id = hl2da_api.SENSOR_ID.RM_VLC_LEFTFRONT; id <= hl2da_api.SENSOR_ID.EXTENDED_EYE_TRACKING; ++id)
        {
            if (id == sync) { continue; }

            // Associate frames
            // If no frame matches fb_ref_timestamp exactly then:
            //   hl2da_api.TIME_PREFERENCE.PAST:    select nearest frame with Timestamp < fb_ref_timestamp
            //   hl2da_api.TIME_PREFERENCE.NEAREST: select nearest frame, in case of a tie choose Timestamp > fb_ref_timestamp if tiebreak_right=true else choose Timestamp < fb_ref_timestamp
            //   hl2da_api.TIME_PREFERENCE.FUTURE:  select nearest frame with Timestamp > fb_ref_timestamp
            hl2da_framebuffer fb = hl2da_framebuffer.GetFrame(id, fb_ref_timestamp, hl2da_api.TIME_PREFERENCE.NEAREST, false);
            if (fb.Status != hl2da_api.STATUS.OK) { continue; }
            Update_Sensor_Data(fb);
            fb.Destroy();
        }
    }

    void Initialize_Dictionaries()
    {
        sensor_names = new Dictionary<hl2da_api.SENSOR_ID, string>();
        last_framestamp = new Dictionary<hl2da_api.SENSOR_ID, int>();

        for (hl2da_api.SENSOR_ID id = hl2da_api.SENSOR_ID.RM_VLC_LEFTFRONT; id <= hl2da_api.SENSOR_ID.EXTENDED_EYE_TRACKING; ++id)
        {
            sensor_names[id] = id.ToString();
            last_framestamp[id] = -1;
        }
    }

    string CentralPoints(hl2da_api.SENSOR_ID id)
    {
        float[,] image_points = new float[1, 2];
        float[,] camera_points = new float[1, 2];

        switch (id)
        {
        case hl2da_api.SENSOR_ID.RM_VLC_LEFTFRONT:
        case hl2da_api.SENSOR_ID.RM_VLC_LEFTLEFT:
        case hl2da_api.SENSOR_ID.RM_VLC_RIGHTFRONT:
        case hl2da_api.SENSOR_ID.RM_VLC_RIGHTRIGHT:  image_points[0, 0] = 320.0f; image_points[0, 1] = 240.0f; break;
        case hl2da_api.SENSOR_ID.RM_DEPTH_AHAT:      image_points[0, 0] = 256.0f; image_points[0, 1] = 256.0f; break;
        case hl2da_api.SENSOR_ID.RM_DEPTH_LONGTHROW: image_points[0, 0] = 160.0f; image_points[0, 1] = 144.0f; break;
        default: return "";
        }

        camera_points[0, 0] = 0.0f;
        camera_points[0, 1] = 0.0f;

        float[,] mpoint = hl2da_user.MapImagePointToCameraUnitPlane_RM(id, image_points); // get image center in camera space
        float[,] ppoint = hl2da_user.MapCameraSpaceToImagePoint_RM(id, camera_points); // get image principal point

        return string.Format(" c'=[{0}, {1}], c=[{2}, {3}]", mpoint[0, 0], mpoint[0, 1], ppoint[0, 0], ppoint[0, 1]);
    }

    void Update_Extrinsics()
    {
        for (hl2da_api.SENSOR_ID id = hl2da_api.SENSOR_ID.RM_VLC_LEFTFRONT; id <= hl2da_api.SENSOR_ID.RM_IMU_GYROSCOPE; ++id)
        {
            float[,] extrinsics = hl2da_user.GetExtrinsics_RM(id);
            string text = sensor_names[id] + " Calibration: extrinsics=" + PoseToString(extrinsics) + CentralPoints(id);
            calibrations[(int)id].GetComponent<TextMeshPro>().text = text;
        }
    }

    void Update_Sensor_Data(hl2da_framebuffer fb)
    {
        if (fb.Framestamp <= last_framestamp[fb.Id]) { return; } // Repeated frame, nothing to do...
        last_framestamp[fb.Id] = fb.Framestamp;

        switch (fb.Id)
        {
        case hl2da_api.SENSOR_ID.RM_VLC_LEFTFRONT:
        case hl2da_api.SENSOR_ID.RM_VLC_LEFTLEFT:
        case hl2da_api.SENSOR_ID.RM_VLC_RIGHTFRONT:
        case hl2da_api.SENSOR_ID.RM_VLC_RIGHTRIGHT:     Update_RM_VLC(fb);               break;
        case hl2da_api.SENSOR_ID.RM_DEPTH_AHAT:         Update_RM_Depth_AHAT(fb);        break;
        case hl2da_api.SENSOR_ID.RM_DEPTH_LONGTHROW:    Update_RM_Depth_Longthrow(fb);   break;
        case hl2da_api.SENSOR_ID.RM_IMU_ACCELEROMETER:  Update_RM_IMU_Accelerometer(fb); break;
        case hl2da_api.SENSOR_ID.RM_IMU_GYROSCOPE:      Update_RM_IMU_Gyroscope(fb);     break;
        case hl2da_api.SENSOR_ID.RM_IMU_MAGNETOMETER:   Update_RM_IMU_Magnetometer(fb);  break;
        case hl2da_api.SENSOR_ID.PV:                    Update_PV(fb);                   break;
        case hl2da_api.SENSOR_ID.MICROPHONE:            Update_Microphone(fb);           break;
        case hl2da_api.SENSOR_ID.SPATIAL_INPUT:         Update_SpatialInput(fb);         break;
        case hl2da_api.SENSOR_ID.EXTENDED_EYE_TRACKING: Update_ExtendedEyeTracking(fb);  break;
        }
    }

    void Update_RM_VLC(hl2da_framebuffer fb)
    {
        int index = (int)fb.Id;

        // Load frame data into textures
        tex_vlc[index].LoadRawTextureData(fb.Buffer(0), fb.Length(0) * sizeof(byte)); // Image is u8
        tex_vlc[index].Apply();

        float[,] pose = hl2da_user.Unpack2D<float>(fb.Buffer(3), hl2da_user.POSE_ROWS, hl2da_user.POSE_COLS);

        // Display frame
        rm_vlc_images[index].GetComponent<Renderer>().material.mainTexture = tex_vlc[index];
        poses[index].GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + " Pose: " + PoseToString(pose);
    }

    void Update_RM_Depth_AHAT(hl2da_framebuffer fb)
    {
        // Load frame data into textures
        tex_ht.LoadRawTextureData(fb.Buffer(0), fb.Length(0) * sizeof(ushort)); // Depth is u16
        tex_ht_ab.LoadRawTextureData(fb.Buffer(1), fb.Length(1) * sizeof(ushort)); // AB is u16
        tex_ht.Apply();
        tex_ht_ab.Apply();

        float[,] pose = hl2da_user.Unpack2D<float>(fb.Buffer(3), hl2da_user.POSE_ROWS, hl2da_user.POSE_COLS);

        // Display frame
        rm_depth_ahat_images[0].GetComponent<Renderer>().material.mainTexture = tex_ht;
        rm_depth_ahat_images[1].GetComponent<Renderer>().material.mainTexture = tex_ht_ab;
        poses[(int)fb.Id].GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + " Pose: " + PoseToString(pose);
    }

    void Update_RM_Depth_Longthrow(hl2da_framebuffer fb)
    {
        // Load frame data into textures
        tex_lt.LoadRawTextureData(fb.Buffer(0), fb.Length(0) * sizeof(ushort)); // Depth is u16
        tex_lt_ab.LoadRawTextureData(fb.Buffer(1), fb.Length(1) * sizeof(ushort)); // AB is u16
        tex_lt_sigma.LoadRawTextureData(fb.Buffer(2), fb.Length(2) * sizeof(byte)); // Sigma is u8
        tex_lt.Apply();
        tex_lt_ab.Apply();
        tex_lt_sigma.Apply();

        float[,] pose = hl2da_user.Unpack2D<float>(fb.Buffer(3), hl2da_user.POSE_ROWS, hl2da_user.POSE_COLS);

        // Display frame
        rm_depth_longthrow_images[0].GetComponent<Renderer>().material.mainTexture = tex_lt;
        rm_depth_longthrow_images[1].GetComponent<Renderer>().material.mainTexture = tex_lt_ab;
        rm_depth_longthrow_images[2].GetComponent<Renderer>().material.mainTexture = tex_lt_sigma;
        poses[(int)fb.Id].GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + " Pose: " + PoseToString(pose);
    }

    void Update_RM_IMU_Accelerometer(hl2da_framebuffer fb)
    {
        // Unpack data
        hl2da_api.AccelDataStruct[] samples = hl2da_user.Unpack1D<hl2da_api.AccelDataStruct>(fb.Buffer(0), fb.Length(0));
        float[,] pose = hl2da_user.Unpack2D<float>(fb.Buffer(3), hl2da_user.POSE_ROWS, hl2da_user.POSE_COLS);

        // Display first sample in the batch
        hl2da_api.AccelDataStruct sample_0 = samples[0];
        acc_text.GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + string.Format(": VinylHupTicks={0}, SocTicks={1}, x={2}, y={3}, z={4}, temperature={5}", sample_0.VinylHupTicks, sample_0.SocTicks, sample_0.x, sample_0.y, sample_0.z, sample_0.temperature);
        poses[(int)fb.Id].GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + " Pose: " + PoseToString(pose);
    }

    void Update_RM_IMU_Gyroscope(hl2da_framebuffer fb)
    {
        // Unpack data
        hl2da_api.GyroDataStruct[] samples = hl2da_user.Unpack1D<hl2da_api.GyroDataStruct>(fb.Buffer(0), fb.Length(0));
        float[,] pose = hl2da_user.Unpack2D<float>(fb.Buffer(3), hl2da_user.POSE_ROWS, hl2da_user.POSE_COLS);

        // Display first sample in the batch
        hl2da_api.GyroDataStruct sample_0 = samples[0];
        gyr_text.GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + string.Format(": VinylHupTicks={0}, SocTicks={1}, x={2}, y={3}, z={4}, temperature={5}", sample_0.VinylHupTicks, sample_0.SocTicks, sample_0.x, sample_0.y, sample_0.z, sample_0.temperature);
        poses[(int)fb.Id].GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + " Pose: " + PoseToString(pose);
    }

    void Update_RM_IMU_Magnetometer(hl2da_framebuffer fb)
    {
        // Unpack data
        hl2da_api.MagDataStruct[] samples = hl2da_user.Unpack1D<hl2da_api.MagDataStruct>(fb.Buffer(0), fb.Length(0));
        float[,] pose = hl2da_user.Unpack2D<float>(fb.Buffer(3), hl2da_user.POSE_ROWS, hl2da_user.POSE_COLS);

        // Display first sample in the batch
        hl2da_api.MagDataStruct sample_0 = samples[0];
        mag_text.GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + string.Format(": VinylHupTicks={0}, SocTicks={1}, x={2}, y={3}, z={4}", sample_0.VinylHupTicks, sample_0.SocTicks, sample_0.x, sample_0.y, sample_0.z);
        poses[(int)fb.Id].GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + " Pose: " + PoseToString(pose);
    }

    void Update_PV(hl2da_framebuffer fb)
    {
        // Load frame data into textures
        tex_pv.LoadRawTextureData(fb.Buffer(0), pvcf.width * pvcf.height * sizeof(byte)); // Image is NV12, load Y only, use OpenCV to convert NV12->RGB
        tex_pv.Apply();

        float[] intrinsics = hl2da_user.Unpack1D<float>(fb.Buffer(2), fb.Length(2)); // fx, fy, cx, cy
        float[,] pose = hl2da_user.Unpack2D<float>(fb.Buffer(3), hl2da_user.POSE_ROWS, hl2da_user.POSE_COLS);

        // Display frame
        pv_image.GetComponent<Renderer>().material.mainTexture = tex_pv;
        poses[(int)fb.Id].GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + " Pose: " + PoseToString(pose);
        calibrations[(int)fb.Id].GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + string.Format(" Calibration: fx={0}, fy={1}, cx={2}, cy={3}", intrinsics[0], intrinsics[1], intrinsics[2], intrinsics[3]);
    }

    void Update_Microphone(hl2da_framebuffer fb)
    {
        // Unpack data
        float[] samples = hl2da_user.Unpack1D<float>(fb.Buffer(0), fb.Length(0));

        // Display first sample for all channels
        string text = mcch switch
        {
            hl2da_api.MC_CHANNELS.USE_2 => string.Format(": ch0={0}, ch1={1}", samples[0], samples[1]),
            hl2da_api.MC_CHANNELS.USE_5 => string.Format(": ch0={0}, ch2={1}, ch3={2}, ch4={3}, ch5={4}", samples[0], samples[1], samples[2], samples[3], samples[4]),
            _ => ": INVALID CONFIGURATION",
        };

        mic_text.GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + text;
    }

    void Update_SpatialInput(hl2da_framebuffer fb)
    {
        // Unpack data
        float[] head_pose = hl2da_user.Unpack1D<float>(fb.Buffer(0), fb.Length(0));
        float[] eye_ray = hl2da_user.Unpack1D<float>(fb.Buffer(1), fb.Length(1));
        hl2da_api.JointPose[] left_hand = hl2da_user.Unpack1D<hl2da_api.JointPose>(fb.Buffer(2), fb.Length(2));
        hl2da_api.JointPose[] right_hand = hl2da_user.Unpack1D<hl2da_api.JointPose>(fb.Buffer(3), fb.Length(3));
        hl2da_api.SI_VALID valid = fb.Valid_SI;

        // Display data
        hl2da_api.JointPose left_wrist = left_hand[(int)hl2da_api.SI_HandJointKind.Wrist];
        hl2da_api.JointPose right_wrist = right_hand[(int)hl2da_api.SI_HandJointKind.Wrist];

        si_text[0].GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + " Head: "        + (((valid & hl2da_api.SI_VALID.HEAD)  != 0) ? string.Format("position=[{0}, {1}, {2}], forward=[{3}, {4}, {5}], up=[{6}, {7}, {8}]", head_pose[0], head_pose[1], head_pose[2], head_pose[3], head_pose[4], head_pose[5], head_pose[6], head_pose[7], head_pose[8]) : "<INVALID>");
        si_text[1].GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + " Eye: "         + (((valid & hl2da_api.SI_VALID.EYE)   != 0) ? string.Format("origin=[{0}, {1}, {2}] direction=[{3}, {4}, {5}]", eye_ray[0], eye_ray[1], eye_ray[2], eye_ray[3], eye_ray[4], eye_ray[5]) : "<INVALID>");
        si_text[2].GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + " Left Wrist: "  + (((valid & hl2da_api.SI_VALID.LEFT)  != 0) ? string.Format("orientation=[{0}, {1}, {2}, {3}], position=[{4}, {5}, {6}], radius={7}, accuracy={8}", left_wrist.rx, left_wrist.ry, left_wrist.rz, left_wrist.rw, left_wrist.tx, left_wrist.ty, left_wrist.tz, left_wrist.radius, left_wrist.accuracy) : "<INVALID>");
        si_text[3].GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + " Right Wrist: " + (((valid & hl2da_api.SI_VALID.RIGHT) != 0) ? string.Format("orientation=[{0}, {1}, {2}, {3}], position=[{4}, {5}, {6}], radius={7}, accuracy={8}", right_wrist.rx, right_wrist.ry, right_wrist.rz, right_wrist.rw, right_wrist.tx, right_wrist.ty, right_wrist.tz, right_wrist.radius, right_wrist.accuracy) : "<INVALID>");
    }

    void Update_ExtendedEyeTracking(hl2da_framebuffer fb)
    {
        // Unpack data
        float[] eye_data = hl2da_user.Unpack1D<float>(fb.Buffer(0), fb.Length(0));
        float[,] pose = hl2da_user.Unpack2D<float>(fb.Buffer(3), hl2da_user.POSE_ROWS, hl2da_user.POSE_COLS);
        hl2da_api.EE_VALID valid = fb.Valid_EE;

        // Display data
        ee_text[0].GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + " Combined Gaze: " + (((valid & hl2da_api.EE_VALID.COMBINED_GAZE) != 0) ? string.Format("origin=[{0}, {1}, {2}] direction=[{3}, {4}, {5}] vergence={6}", eye_data[ 0], eye_data[ 1], eye_data[ 2], eye_data[ 3], eye_data[ 4], eye_data[ 5], eye_data[20]) : "<INVALID>");
        ee_text[1].GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + " Left Gaze: "     + (((valid & hl2da_api.EE_VALID.LEFT_GAZE)     != 0) ? string.Format("origin=[{0}, {1}, {2}] direction=[{3}, {4}, {5}] openness={6}", eye_data[ 6], eye_data[ 7], eye_data[ 8], eye_data[ 9], eye_data[10], eye_data[11], eye_data[18]) : "<INVALID>");
        ee_text[2].GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + " Right Gaze: "    + (((valid & hl2da_api.EE_VALID.RIGHT_GAZE)    != 0) ? string.Format("origin=[{0}, {1}, {2}] direction=[{3}, {4}, {5}] openness={6}", eye_data[12], eye_data[13], eye_data[14], eye_data[15], eye_data[16], eye_data[17], eye_data[19]) : "<INVALID>");
        
        poses[(int)fb.Id].GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + " Pose: " + PoseToString(pose) + (((valid & hl2da_api.EE_VALID.CALIBRATION) != 0) ? "" : " <BAD CALIBRATION>");
    }

    string PoseToString(float[,] pose)
    {
        return string.Format("[[{0}, {1}, {2}, {3}], [{4}, {5}, {6}, {7}], [{8}, {9}, {10}, {11}], [{12}, {13}, {14}, {15}]]", pose[0, 0], pose[0, 1], pose[0, 2], pose[0, 3], pose[1, 0], pose[1, 1], pose[1, 2], pose[1, 3], pose[2, 0], pose[2, 1], pose[2, 2], pose[2, 3], pose[3, 0], pose[3, 1], pose[3, 2], pose[3, 3]);
    }
}
