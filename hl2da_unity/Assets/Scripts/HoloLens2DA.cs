
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
    public GameObject ev_image;
    public GameObject mic_text;
    public GameObject[] si_text;
    public GameObject[] ee_text;
    public GameObject ea_text;
    public GameObject ev_text;

    public Texture colormap_texture;
    public Shader colormap_shader;
    public Shader sqrtmap_shader;
    public Shader grayscale_shader;

    private Texture2D[] tex_vlc;
    private Texture2D tex_ht;
    private Texture2D tex_ht_ab;
    private Texture2D tex_lt;
    private Texture2D tex_lt_ab;
    private Texture2D tex_lt_sigma;
    private Texture2D tex_pv;
    private Texture2D tex_ev;

    private RenderTexture[] tex_vlc_r;
    private RenderTexture tex_ht_r;
    private RenderTexture tex_ht_ab_r;
    private RenderTexture tex_lt_r;
    private RenderTexture tex_lt_ab_r;
    private RenderTexture tex_lt_sigma_r;

    private Material colormap_mat_ht;
    private Material colormap_mat_lt;
    private Material sqrtmap_mat;
    private Material grayscale_mat;

    private Dictionary<hl2da.SENSOR_ID, string> sensor_names;
    private Dictionary<hl2da.SENSOR_ID, int> last_framestamp;
    private hl2da.SENSOR_ID sync;
    private hl2da.pv_captureformat pvcf;
    private hl2da.ea_captureformat eacf;
    private hl2da.ev_captureformat evcf;
    private hl2da.MC_CHANNELS mcch;
    private hl2da.EE_FPS_INDEX eefi;
    private bool invalidate_depth;
    private ulong utc_offset;
    private bool pv_settings_latch;

    private TextMeshPro[] tmp_vlc_pose;
    private TextMeshPro tmp_ht_pose;
    private TextMeshPro tmp_lt_pose;
    private TextMeshPro tmp_acc_pose;
    private TextMeshPro tmp_gyr_pose;
    private TextMeshPro tmp_mag_pose;
    private TextMeshPro tmp_pv_pose;
    private TextMeshPro tmp_pv_k;
    private TextMeshPro tmp_mic;
    private TextMeshPro tmp_si_head;
    private TextMeshPro tmp_si_eye;
    private TextMeshPro tmp_si_left;
    private TextMeshPro tmp_si_right;
    private TextMeshPro tmp_ee_combined;
    private TextMeshPro tmp_ee_left;
    private TextMeshPro tmp_ee_right;
    private TextMeshPro tmp_ee_pose;
    private TextMeshPro tmp_ea;
    private TextMeshPro tmp_ev_format;

    private Dictionary<hl2da.SENSOR_ID, float[,]> rm_uv2xy = new Dictionary<hl2da.SENSOR_ID, float[,]>();
    private Dictionary<hl2da.SENSOR_ID, float[,]> rm_mapxy = new Dictionary<hl2da.SENSOR_ID, float[,]>();
    private Dictionary<hl2da.SENSOR_ID, float[]> rm_intrinsics = new Dictionary<hl2da.SENSOR_ID, float[]>();

    // Start is called before the first frame update
    void Start()
    {
        // Set invalid depth pixels to zero
        invalidate_depth = true;

        // PV format
        // List of supported resolutions and framerates at https://github.com/jdibenes/hl2ss/blob/main/etc/pv_configurations.txt
        // enable_mrc: Enable Mixed Reality Capture (holograms in video)
        // shared: Enable Shared Mode (for when other apps or modules are already using the PV camera, resolution and framerate parameters are ignored)
        pvcf = hl2da.user.CreateFormat_PV(760, 428, 30, false, false);

        // Additional PV capture settings
        // To change the focus mode, temporal denoising, white balance, exposure, scene mode, iso speed, or backlight compensation,
        // after calling Initialize(PV, buffer_size) and SetEnable(PV, true),
        // wait to receive at least one frame (from hl2da.framebuffer.GetFrame with hl2da.STATUS.OK),
        // then these settings can be changed freely as long as the stream is enabled.
        // Unlike PV format, these settings are not latched internally, so they will be lost if the stream is restarted (via SetEnable(PV, false) -> SetEnable(PV, true))
        // and they must be reapplied using the SetEnable(PV, true) -> Wait one frame procedure.

        // Microphone format
        // hl2da.MC_CHANNELS.USE_2: Preprocessed 2-channel audio
        // hl2da.MC_CHANNELS.USE_5: Raw audio from the 5-channel HoloLens microphone array
        mcch = hl2da.MC_CHANNELS.USE_5;

        // Extended Eye Tracking framerate
        eefi = hl2da.EE_FPS_INDEX.FPS_90;

        // Extended Audio format
        // See https://github.com/jdibenes/hl2ss/blob/main/viewer/client_stream_extended_audio.py for how to find the device_index for your microphone
        // source_index = 0 should work for most microphones but different values > 0 allow to select different formats if any
        eacf = hl2da.user.CreateFormat_EA(hl2da.MIXER_MODE.MICROPHONE, 2, 0); // Select external microphone if any

        // Extended Video format
        // See https://github.com/jdibenes/hl2ss/blob/main/viewer/client_stream_extended_video.py for how to find the parameters for your camera
        evcf = hl2da.user.CreateFormat_EV(1280, 720, 30, "YUY2", false, 2, 0, 0); // Select external camera if any

        // Synchronize to sensor (value not in hl2da.SENSOR_ID = no synchronization)
        sync = (hl2da.SENSOR_ID)(-1); // hl2da.SENSOR_ID.RM_DEPTH_LONGTHROW;
        
        Initialize_Dictionaries();

        hl2da.user.InitializeComponents();
        hl2da.user.OverrideWorldCoordinateSystem(); // Link Unity and plugin coordinate systems

        hl2da.user.RM_SetEyeSelection(true); // ???
        hl2da.user.SetFormat_PV(pvcf);
        hl2da.user.SetFormat_Microphone(mcch);
        hl2da.user.SetFormat_ExtendedEyeTracking(eefi);
        hl2da.user.SetFormat_ExtendedAudio(eacf);
        hl2da.user.SetFormat_ExtendedVideo(evcf);

        utc_offset = hl2da.user.GetUTCOffset();
        pv_settings_latch = false;

        Update_Calibration();

        hl2da.user.SetConstantFactorVLC_RM(); // Workaround for https://github.com/microsoft/HoloLens2ForCV/issues/134
        hl2da.user.BypassDepthLock_RM(true); // Allows simultaneous access to AHAT and longthrow depth

        // Test, not required
        hl2da.user.EX_SetInterfacePriority(hl2da.SENSOR_ID.RM_VLC_LEFTFRONT, hl2da.InterfacePriority.ABOVE_NORMAL);
        hl2da.user.EX_SetInterfacePriority(hl2da.SENSOR_ID.RM_VLC_LEFTFRONT, hl2da.InterfacePriority.ABOVE_NORMAL);
        int pv_prio = hl2da.user.EX_GetInterfacePriority(hl2da.SENSOR_ID.PV);

        // Use a buffer size of 2 seconds (except longthrow and PV)
        hl2da.user.Initialize(hl2da.SENSOR_ID.RM_VLC_LEFTFRONT,       60); // Buffer size limited by memory                          // 30 Hz
        hl2da.user.Initialize(hl2da.SENSOR_ID.RM_VLC_LEFTLEFT,        60); // Buffer size limited by memory                          // 30 Hz
        hl2da.user.Initialize(hl2da.SENSOR_ID.RM_VLC_RIGHTFRONT,      60); // Buffer size limited by memory                          // 30 Hz
        hl2da.user.Initialize(hl2da.SENSOR_ID.RM_VLC_RIGHTRIGHT,      60); // Buffer size limited by memory                          // 30 Hz
        hl2da.user.Initialize(hl2da.SENSOR_ID.RM_DEPTH_AHAT,          90); // Buffer size limited by memory                          // 45 Hz
        hl2da.user.Initialize(hl2da.SENSOR_ID.RM_DEPTH_LONGTHROW,     15); // Buffer size limited by internal buffer - Maximum is 18 // 5 Hz
        hl2da.user.Initialize(hl2da.SENSOR_ID.RM_IMU_ACCELEROMETER,   24); // Buffer size limited by memory                          // ~12 Hz per batch
        hl2da.user.Initialize(hl2da.SENSOR_ID.RM_IMU_GYROSCOPE,       48); // Buffer size limited by memory                          // ~24 Hz per batch
        hl2da.user.Initialize(hl2da.SENSOR_ID.RM_IMU_MAGNETOMETER,    10); // Buffer size limited by memory                          // ~5 Hz per batch
        hl2da.user.Initialize(hl2da.SENSOR_ID.PV,                     15); // Buffer size limited by internal buffer - Maximum is 18 // 5, 15, 30, 60 Hz selectable depending on chosen resolution 
        hl2da.user.Initialize(hl2da.SENSOR_ID.MICROPHONE,            125); // Buffer size limited by memory                          // 768 samples per frame equivalent to 768/48000=16 ms of audio
        hl2da.user.Initialize(hl2da.SENSOR_ID.SPATIAL_INPUT,          60); // Buffer size limited by memory                          // 30 Hz
        hl2da.user.Initialize(hl2da.SENSOR_ID.EXTENDED_EYE_TRACKING, 180); // Buffer size limited by memory                          // 30, 60, or 90 Hz selectable
        hl2da.user.Initialize(hl2da.SENSOR_ID.EXTENDED_AUDIO,        125); // Buffer size limited by memory                          // audio length per frame depends on your microphone
        hl2da.user.Initialize(hl2da.SENSOR_ID.EXTENDED_VIDEO,         15); // Buffer size limited by internal buffer - Maximum is 18 // framerate depends on your camera

        hl2da.user.SetEnable(hl2da.SENSOR_ID.RM_VLC_LEFTFRONT,      true);
        hl2da.user.SetEnable(hl2da.SENSOR_ID.RM_VLC_LEFTLEFT,       true);
        hl2da.user.SetEnable(hl2da.SENSOR_ID.RM_VLC_RIGHTFRONT,     true);
        hl2da.user.SetEnable(hl2da.SENSOR_ID.RM_VLC_RIGHTRIGHT,     true);
        hl2da.user.SetEnable(hl2da.SENSOR_ID.RM_DEPTH_AHAT,         true);
        hl2da.user.SetEnable(hl2da.SENSOR_ID.RM_DEPTH_LONGTHROW,    true);
        hl2da.user.SetEnable(hl2da.SENSOR_ID.RM_IMU_ACCELEROMETER,  true);
        hl2da.user.SetEnable(hl2da.SENSOR_ID.RM_IMU_GYROSCOPE,      true);
        hl2da.user.SetEnable(hl2da.SENSOR_ID.RM_IMU_MAGNETOMETER,   true);
        hl2da.user.SetEnable(hl2da.SENSOR_ID.PV,                    true);
        hl2da.user.SetEnable(hl2da.SENSOR_ID.MICROPHONE,            true);
        hl2da.user.SetEnable(hl2da.SENSOR_ID.SPATIAL_INPUT,         true);
        hl2da.user.SetEnable(hl2da.SENSOR_ID.EXTENDED_EYE_TRACKING, true);
        hl2da.user.SetEnable(hl2da.SENSOR_ID.EXTENDED_AUDIO,        true);
        hl2da.user.SetEnable(hl2da.SENSOR_ID.EXTENDED_VIDEO,        true);

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

        tex_pv = new Texture2D((int)hl2da.converter.GetStride_PV(pvcf.width), pvcf.height, TextureFormat.BGRA32, false);
        tex_ev = new Texture2D(evcf.width, evcf.height, TextureFormat.BGRA32, false);

        tex_vlc_r = new RenderTexture[4];

        tex_vlc_r[0]   = new RenderTexture(640, 480, 0, RenderTextureFormat.BGRA32);
        tex_vlc_r[1]   = new RenderTexture(640, 480, 0, RenderTextureFormat.BGRA32);
        tex_vlc_r[2]   = new RenderTexture(640, 480, 0, RenderTextureFormat.BGRA32);
        tex_vlc_r[3]   = new RenderTexture(640, 480, 0, RenderTextureFormat.BGRA32);
        tex_ht_r       = new RenderTexture(512, 512, 0, RenderTextureFormat.BGRA32);
        tex_ht_ab_r    = new RenderTexture(512, 512, 0, RenderTextureFormat.BGRA32);
        tex_lt_r       = new RenderTexture(320, 288, 0, RenderTextureFormat.BGRA32);
        tex_lt_ab_r    = new RenderTexture(320, 288, 0, RenderTextureFormat.BGRA32);
        tex_lt_sigma_r = new RenderTexture(320, 288, 0, RenderTextureFormat.BGRA32);

        colormap_mat_ht = new Material(colormap_shader);
        colormap_mat_lt = new Material(colormap_shader);
        sqrtmap_mat     = new Material(sqrtmap_shader);
        grayscale_mat   = new Material(grayscale_shader);

        colormap_mat_ht.SetTexture("_ColorMapTex", colormap_texture);
        colormap_mat_ht.SetFloat("_Lf", 0.0f / 65535.0f);
        colormap_mat_ht.SetFloat("_Rf", 1055.0f / 65535.0f);

        colormap_mat_lt.SetTexture("_ColorMapTex", colormap_texture);
        colormap_mat_lt.SetFloat("_Lf", 0.0f / 65535.0f);
        colormap_mat_lt.SetFloat("_Rf", 3000.0f / 65535.0f);

        rm_vlc_images[0].GetComponent<Renderer>().material.mainTexture = tex_vlc_r[0];
        rm_vlc_images[1].GetComponent<Renderer>().material.mainTexture = tex_vlc_r[1];
        rm_vlc_images[2].GetComponent<Renderer>().material.mainTexture = tex_vlc_r[2];
        rm_vlc_images[3].GetComponent<Renderer>().material.mainTexture = tex_vlc_r[3];

        rm_depth_ahat_images[0].GetComponent<Renderer>().material.mainTexture = tex_ht_r;
        rm_depth_ahat_images[1].GetComponent<Renderer>().material.mainTexture = tex_ht_ab_r;

        rm_depth_longthrow_images[0].GetComponent<Renderer>().material.mainTexture = tex_lt_r;
        rm_depth_longthrow_images[1].GetComponent<Renderer>().material.mainTexture = tex_lt_ab_r;
        rm_depth_longthrow_images[2].GetComponent<Renderer>().material.mainTexture = tex_lt_sigma_r;

        pv_image.GetComponent<Renderer>().material.mainTexture = tex_pv;

        ev_image.GetComponent<Renderer>().material.mainTexture = tex_ev;

        tmp_vlc_pose = new TextMeshPro[4];

        tmp_vlc_pose[0] = poses[0].GetComponent<TextMeshPro>();
        tmp_vlc_pose[1] = poses[1].GetComponent<TextMeshPro>();
        tmp_vlc_pose[2] = poses[2].GetComponent<TextMeshPro>();
        tmp_vlc_pose[3] = poses[3].GetComponent<TextMeshPro>();

        tmp_ht_pose = poses[(int)hl2da.SENSOR_ID.RM_DEPTH_AHAT].GetComponent<TextMeshPro>();
        tmp_lt_pose = poses[(int)hl2da.SENSOR_ID.RM_DEPTH_LONGTHROW].GetComponent<TextMeshPro>();

        tmp_acc_pose = poses[(int)hl2da.SENSOR_ID.RM_IMU_ACCELEROMETER].GetComponent<TextMeshPro>();
        tmp_gyr_pose = poses[(int)hl2da.SENSOR_ID.RM_IMU_GYROSCOPE].GetComponent<TextMeshPro>();
        tmp_mag_pose = poses[(int)hl2da.SENSOR_ID.RM_IMU_MAGNETOMETER].GetComponent<TextMeshPro>();

        tmp_pv_pose = poses[(int)hl2da.SENSOR_ID.PV].GetComponent<TextMeshPro>();
        tmp_pv_k = calibrations[(int)hl2da.SENSOR_ID.PV].GetComponent<TextMeshPro>();

        tmp_mic = mic_text.GetComponent<TextMeshPro>();

        tmp_si_head  = si_text[0].GetComponent<TextMeshPro>();
        tmp_si_eye   = si_text[1].GetComponent<TextMeshPro>();
        tmp_si_left  = si_text[2].GetComponent<TextMeshPro>();
        tmp_si_right = si_text[3].GetComponent<TextMeshPro>();

        tmp_ee_combined = ee_text[0].GetComponent<TextMeshPro>();
        tmp_ee_left = ee_text[1].GetComponent<TextMeshPro>();
        tmp_ee_right = ee_text[2].GetComponent<TextMeshPro>();
        tmp_ee_pose = poses[(int)hl2da.SENSOR_ID.EXTENDED_EYE_TRACKING].GetComponent<TextMeshPro>();

        tmp_ea = ea_text.GetComponent<TextMeshPro>();
        tmp_ev_format = ev_text.GetComponent<TextMeshPro>();
    }

    // Update is called once per frame
    void Update()
    {
        if ((sync < hl2da.SENSOR_ID.RM_VLC_LEFTFRONT) || (sync > hl2da.SENSOR_ID.EXTENDED_VIDEO))
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
        for (hl2da.SENSOR_ID id = hl2da.SENSOR_ID.RM_VLC_LEFTFRONT; id <= hl2da.SENSOR_ID.EXTENDED_VIDEO; ++id)
        {
            // Get Frame
            // Negative index (-n): get nth most recent frame, may repeat or skip frames as necessary
            // Non-negative index (n>=0): get nth frame where 0 is the first frame ever since SetEnable(true), intended for sequential access (no skip or repeat)
            using (hl2da.framebuffer fb = hl2da.framebuffer.GetFrame(id, -1)) // Get most recent frame
            {
                // Check result
                // DISCARDED: requested frame is too old and has been removed from the internal buffer (cannot be recovered)
                // OK: got requested frame
                // WAIT: requested frame has not been captured yet (just wait, ideally 1/frame_rate seconds)
                if (fb.Status != hl2da.STATUS.OK) { continue; }

                Update_Sensor_Data(fb);
            }
        }
    }

    void Update_Sync()
    {
        ulong fb_ref_timestamp;
        using (hl2da.framebuffer fb_ref = hl2da.framebuffer.GetFrame(sync, -2)) // Use a small delay to allow receiving the optimal frame from the second stream
        {
            if (fb_ref.Status != hl2da.STATUS.OK) { return; }
            fb_ref_timestamp = fb_ref.Timestamp;
            Update_Sensor_Data(fb_ref);
        }

        for (hl2da.SENSOR_ID id = hl2da.SENSOR_ID.RM_VLC_LEFTFRONT; id <= hl2da.SENSOR_ID.EXTENDED_VIDEO; ++id)
        {
            if (id == sync) { continue; }

            // Associate frames
            // If no frame matches fb_ref_timestamp exactly then:
            //   hl2da.TIME_PREFERENCE.PAST:    select nearest frame with Timestamp < fb_ref_timestamp
            //   hl2da.TIME_PREFERENCE.NEAREST: select nearest frame, in case of a tie choose Timestamp > fb_ref_timestamp if tiebreak_right=true else choose Timestamp < fb_ref_timestamp
            //   hl2da.TIME_PREFERENCE.FUTURE:  select nearest frame with Timestamp > fb_ref_timestamp
            using (hl2da.framebuffer fb = hl2da.framebuffer.GetFrame(id, fb_ref_timestamp, hl2da.TIME_PREFERENCE.NEAREST, false))
            {
                if (fb.Status != hl2da.STATUS.OK) { continue; }
                Update_Sensor_Data(fb);
            }
        }
    }

    void Initialize_Dictionaries()
    {
        sensor_names = new Dictionary<hl2da.SENSOR_ID, string>();
        last_framestamp = new Dictionary<hl2da.SENSOR_ID, int>();

        for (hl2da.SENSOR_ID id = hl2da.SENSOR_ID.RM_VLC_LEFTFRONT; id <= hl2da.SENSOR_ID.EXTENDED_VIDEO; ++id)
        {
            sensor_names[id] = id.ToString();
            last_framestamp[id] = -1;
        }
    }

    string CentralPoints(hl2da.SENSOR_ID id)
    {
        float[,] image_points = new float[1, 2];
        float[,] camera_points = new float[1, 2];

        switch (id)
        {
        case hl2da.SENSOR_ID.RM_VLC_LEFTFRONT:
        case hl2da.SENSOR_ID.RM_VLC_LEFTLEFT:
        case hl2da.SENSOR_ID.RM_VLC_RIGHTFRONT:
        case hl2da.SENSOR_ID.RM_VLC_RIGHTRIGHT:  image_points[0, 0] = 320.0f; image_points[0, 1] = 240.0f; break;
        case hl2da.SENSOR_ID.RM_DEPTH_AHAT:      image_points[0, 0] = 256.0f; image_points[0, 1] = 256.0f; break;
        case hl2da.SENSOR_ID.RM_DEPTH_LONGTHROW: image_points[0, 0] = 160.0f; image_points[0, 1] = 144.0f; break;
        default: return "";
        }

        camera_points[0, 0] = 0.0f;
        camera_points[0, 1] = 0.0f;

        float[,] mpoint = hl2da.user.RM_MapImagePointToCameraUnitPlane(id, image_points); // get image center in camera space
        float[,] ppoint = hl2da.user.RM_MapCameraSpaceToImagePoint(id, camera_points); // get image principal point

        return string.Format(" c'=[{0}, {1}], c=[{2}, {3}]", mpoint[0, 0], mpoint[0, 1], ppoint[0, 0], ppoint[0, 1]);
    }

    string Calibration(hl2da.SENSOR_ID id)
    {
        switch (id)
        {
        case hl2da.SENSOR_ID.RM_VLC_LEFTFRONT:
        case hl2da.SENSOR_ID.RM_VLC_LEFTLEFT:
        case hl2da.SENSOR_ID.RM_VLC_RIGHTFRONT:
        case hl2da.SENSOR_ID.RM_VLC_RIGHTRIGHT:
        case hl2da.SENSOR_ID.RM_DEPTH_AHAT:
        case hl2da.SENSOR_ID.RM_DEPTH_LONGTHROW: break;
        default: return "";
        }

        hl2da.user.RM_GetIntrinsics(id, out float[,] uv2xy, out float[,] mapxy, out float[] k);

        rm_uv2xy[id] = uv2xy;
        rm_mapxy[id] = mapxy;
        rm_intrinsics[id] = k;

        return string.Format(" fx={0}, fy={1}, cx={2}, cy={3}", k[0], k[1], k[2], k[3]);
    }

    void Update_Calibration()
    {
        for (hl2da.SENSOR_ID id = hl2da.SENSOR_ID.RM_VLC_LEFTFRONT; id <= hl2da.SENSOR_ID.RM_IMU_GYROSCOPE; ++id)
        {
            float[,] extrinsics = hl2da.user.RM_GetExtrinsics(id);
            string text = sensor_names[id] + " Calibration: extrinsics=" + PoseToString(extrinsics) + CentralPoints(id) + Calibration(id);
            calibrations[(int)id].GetComponent<TextMeshPro>().text = text;
        }
    }

    void Update_Sensor_Data(hl2da.framebuffer fb)
    {
        if (fb.Framestamp <= last_framestamp[fb.Id]) { return; } // Repeated frame, nothing to do...
        last_framestamp[fb.Id] = fb.Framestamp;

        switch (fb.Id)
        {
        case hl2da.SENSOR_ID.RM_VLC_LEFTFRONT:
        case hl2da.SENSOR_ID.RM_VLC_LEFTLEFT:
        case hl2da.SENSOR_ID.RM_VLC_RIGHTFRONT:
        case hl2da.SENSOR_ID.RM_VLC_RIGHTRIGHT:     Update_RM_VLC(fb);               break;
        case hl2da.SENSOR_ID.RM_DEPTH_AHAT:         Update_RM_Depth_AHAT(fb);        break;
        case hl2da.SENSOR_ID.RM_DEPTH_LONGTHROW:    Update_RM_Depth_Longthrow(fb);   break;
        case hl2da.SENSOR_ID.RM_IMU_ACCELEROMETER:  Update_RM_IMU_Accelerometer(fb); break;
        case hl2da.SENSOR_ID.RM_IMU_GYROSCOPE:      Update_RM_IMU_Gyroscope(fb);     break;
        case hl2da.SENSOR_ID.RM_IMU_MAGNETOMETER:   Update_RM_IMU_Magnetometer(fb);  break;
        case hl2da.SENSOR_ID.PV:                    Update_PV(fb);                   break;
        case hl2da.SENSOR_ID.MICROPHONE:            Update_Microphone(fb);           break;
        case hl2da.SENSOR_ID.SPATIAL_INPUT:         Update_SpatialInput(fb);         break;
        case hl2da.SENSOR_ID.EXTENDED_EYE_TRACKING: Update_ExtendedEyeTracking(fb);  break;
        case hl2da.SENSOR_ID.EXTENDED_AUDIO:        Update_ExtendedAudio(fb);        break;
        case hl2da.SENSOR_ID.EXTENDED_VIDEO:        Update_ExtendedVideo(fb);        break;
        }
    }

    void Update_RM_VLC(hl2da.framebuffer fb)
    {
        int index = (int)fb.Id;

        // Load frame data into textures
        //byte[,] undistorted_image = hl2da_coprocessor.RM_Undistort<byte>(fb.Id, rm_mapxy[fb.Id], 1, 0, 0, fb.Buffer(0));
        //GCHandle h2 = GCHandle.Alloc(undistorted_image, GCHandleType.Pinned);
        //tex_vlc[index].LoadRawTextureData(h2.AddrOfPinnedObject(), fb.Length(0) * sizeof(byte));
        //h2.Free();
        tex_vlc[index].LoadRawTextureData(fb.Buffer(0), fb.Length(0) * sizeof(byte));  // Image is u8
        tex_vlc[index].Apply();
        Graphics.Blit(tex_vlc[index], tex_vlc_r[index], grayscale_mat); // Apply grayscale map to Image

        var metadata = hl2da.user.Unpack<hl2da.vlc_metadata>(fb.Buffer(2));

        // Display pose
        float[,] pose = hl2da.user.Unpack2D<float>(fb.Buffer(3), hl2da.user.POSE_ROWS, hl2da.user.POSE_COLS);
        tmp_vlc_pose[index].text = sensor_names[fb.Id] + string.Format(" exposure={0}, gain={1}", metadata.exposure, metadata.gain) + " Pose: " + PoseToString(pose);
    }

    void Update_RM_Depth_AHAT(hl2da.framebuffer fb)
    {
        if (invalidate_depth) { hl2da.IMT_ZHTInvalidate(fb.Buffer(0), fb.Buffer(0)); }

        // Load frame data into textures
        tex_ht.LoadRawTextureData(fb.Buffer(0), fb.Length(0) * sizeof(ushort));  // Depth is u16
        tex_ht_ab.LoadRawTextureData(fb.Buffer(1), fb.Length(1) * sizeof(ushort));  // AB is u16
        tex_ht.Apply();
        tex_ht_ab.Apply();
        Graphics.Blit(tex_ht, tex_ht_r, colormap_mat_ht); // Apply color map to Depth
        Graphics.Blit(tex_ht_ab, tex_ht_ab_r, sqrtmap_mat); // Apply sqrt map to AB for visibility

        // Display pose
        float[,] pose = hl2da.user.Unpack2D<float>(fb.Buffer(3), hl2da.user.POSE_ROWS, hl2da.user.POSE_COLS);
        tmp_ht_pose.text = sensor_names[fb.Id] + " Pose: " + PoseToString(pose);
    }

    void Update_RM_Depth_Longthrow(hl2da.framebuffer fb)
    {
        if (invalidate_depth) { hl2da.IMT_ZLTInvalidate(fb.Buffer(2), fb.Buffer(0), fb.Buffer(0)); }

        // Load frame data into textures
        tex_lt.LoadRawTextureData(fb.Buffer(0), fb.Length(0) * sizeof(ushort)); // Depth is u16
        tex_lt_ab.LoadRawTextureData(fb.Buffer(1), fb.Length(1) * sizeof(ushort)); // AB is u16
        tex_lt_sigma.LoadRawTextureData(fb.Buffer(2), fb.Length(2) * sizeof(byte)); // Sigma is u8
        tex_lt.Apply();
        tex_lt_ab.Apply();
        tex_lt_sigma.Apply();
        Graphics.Blit(tex_lt, tex_lt_r, colormap_mat_lt); // Apply color map to Depth
        Graphics.Blit(tex_lt_ab, tex_lt_ab_r, sqrtmap_mat); // Apply sqrt map to AB for visibility
        Graphics.Blit(tex_lt_sigma, tex_lt_sigma_r, grayscale_mat); // Apply grayscale map to sigma

        // Display pose
        float[,] pose = hl2da.user.Unpack2D<float>(fb.Buffer(3), hl2da.user.POSE_ROWS, hl2da.user.POSE_COLS);        
        tmp_lt_pose.text = sensor_names[fb.Id] + " Pose: " + PoseToString(pose);
    }

    void Update_RM_IMU_Accelerometer(hl2da.framebuffer fb)
    {
        // Unpack data
        hl2da.AccelDataStruct[] samples = hl2da.user.Unpack1D<hl2da.AccelDataStruct>(fb.Buffer(0), fb.Length(0));
        float[,] pose = hl2da.user.Unpack2D<float>(fb.Buffer(3), hl2da.user.POSE_ROWS, hl2da.user.POSE_COLS);

        // Display first sample in the batch
        hl2da.AccelDataStruct sample_0 = samples[0];
        acc_text.GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + string.Format(": VinylHupTicks={0}, SocTicks={1}, x={2}, y={3}, z={4}, temperature={5}", sample_0.VinylHupTicks, sample_0.SocTicks, sample_0.x, sample_0.y, sample_0.z, sample_0.temperature);
        tmp_acc_pose.text = sensor_names[fb.Id] + " Pose: " + PoseToString(pose);
    }

    void Update_RM_IMU_Gyroscope(hl2da.framebuffer fb)
    {
        // Unpack data
        hl2da.GyroDataStruct[] samples = hl2da.user.Unpack1D<hl2da.GyroDataStruct>(fb.Buffer(0), fb.Length(0));
        float[,] pose = hl2da.user.Unpack2D<float>(fb.Buffer(3), hl2da.user.POSE_ROWS, hl2da.user.POSE_COLS);

        // Display first sample in the batch
        hl2da.GyroDataStruct sample_0 = samples[0];
        gyr_text.GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + string.Format(": VinylHupTicks={0}, SocTicks={1}, x={2}, y={3}, z={4}, temperature={5}", sample_0.VinylHupTicks, sample_0.SocTicks, sample_0.x, sample_0.y, sample_0.z, sample_0.temperature);
        tmp_gyr_pose.text = sensor_names[fb.Id] + " Pose: " + PoseToString(pose);
    }

    void Update_RM_IMU_Magnetometer(hl2da.framebuffer fb)
    {
        // Unpack data
        hl2da.MagDataStruct[] samples = hl2da.user.Unpack1D<hl2da.MagDataStruct>(fb.Buffer(0), fb.Length(0));
        float[,] pose = hl2da.user.Unpack2D<float>(fb.Buffer(3), hl2da.user.POSE_ROWS, hl2da.user.POSE_COLS);

        // Display first sample in the batch
        hl2da.MagDataStruct sample_0 = samples[0];
        mag_text.GetComponent<TextMeshPro>().text = sensor_names[fb.Id] + string.Format(": VinylHupTicks={0}, SocTicks={1}, x={2}, y={3}, z={4}", sample_0.VinylHupTicks, sample_0.SocTicks, sample_0.x, sample_0.y, sample_0.z);
        tmp_mag_pose.text = sensor_names[fb.Id] + " Pose: " + PoseToString(pose);
    }

    void Update_PV(hl2da.framebuffer fb)
    {
        /*
        if (!pv_settings_latch)
        {
            TestPVSettings();
            pv_settings_latch = true;
        }
        */

        // Load frame data into textures
        using (hl2da.converter fc = hl2da.converter.Convert(fb.Buffer(0), hl2da.converter.GetStride_PV(pvcf.width), pvcf.height, hl2da.IMT_Format.Nv12, hl2da.IMT_Format.Bgra8)) // PV images are NV12
        {
            tex_pv.LoadRawTextureData(fc.Buffer, fc.Length);
            tex_pv.Apply();
        }

        var metadata = hl2da.user.Unpack<hl2da.pv_metadata>(fb.Buffer(2));
        //float[,] pose = hl2da.user.Unpack2D<float>(fb.Buffer(3), hl2da.user.POSE_ROWS, hl2da.user.POSE_COLS);
        Matrix4x4 pose = hl2da.user.Unpack<Matrix4x4>(fb.Buffer(3));

        // Display frame        
        tmp_pv_pose.text = sensor_names[fb.Id] + " Pose: " + PoseToString(pose);
        tmp_pv_k.text = sensor_names[fb.Id] + string.Format(" Calibration: fx={0}, fy={1}, cx={2}, cy={3}, tx={4}, ty={5}, tz={6}, rx={7}, ry={8}, rz={9}, rw={10}", metadata.fx, metadata.fy, metadata.cx, metadata.cy, metadata.extrinsics_mf_tx, metadata.extrinsics_mf_ty, metadata.extrinsics_mf_tz, metadata.extrinsics_mf_rx, metadata.extrinsics_mf_ry, metadata.extrinsics_mf_rz, metadata.extrinsics_mf_rw);
    }

    void Update_Microphone(hl2da.framebuffer fb)
    {
        // Unpack data
        float[] samples = hl2da.user.Unpack1D<float>(fb.Buffer(0), fb.Length(0));

        // Display first sample for all channels
        string text = mcch switch
        {
            hl2da.MC_CHANNELS.USE_2 => string.Format(": ch0={0}, ch1={1}", samples[0], samples[1]),
            hl2da.MC_CHANNELS.USE_5 => string.Format(": ch0={0}, ch2={1}, ch3={2}, ch4={3}, ch5={4}", samples[0], samples[1], samples[2], samples[3], samples[4]),
            _ => ": INVALID CONFIGURATION",
        };

        tmp_mic.text = sensor_names[fb.Id] + text;
    }

    void Update_SpatialInput(hl2da.framebuffer fb)
    {
        // Unpack data
        float[] head_pose = hl2da.user.Unpack1D<float>(fb.Buffer(0), fb.Length(0));
        float[] eye_ray = hl2da.user.Unpack1D<float>(fb.Buffer(1), fb.Length(1));
        hl2da.JointPose[] left_hand = hl2da.user.Unpack1D<hl2da.JointPose>(fb.Buffer(2), fb.Length(2));
        hl2da.JointPose[] right_hand = hl2da.user.Unpack1D<hl2da.JointPose>(fb.Buffer(3), fb.Length(3));
        hl2da.SI_VALID valid = fb.Valid_SI;

        // Display data
        hl2da.JointPose left_wrist = left_hand[(int)hl2da.SI_HandJointKind.Wrist];
        hl2da.JointPose right_wrist = right_hand[(int)hl2da.SI_HandJointKind.Wrist];

        tmp_si_head.text  = sensor_names[fb.Id] + " Head: "        + (((valid & hl2da.SI_VALID.HEAD)  != 0) ? string.Format("position=[{0}, {1}, {2}], forward=[{3}, {4}, {5}], up=[{6}, {7}, {8}]", head_pose[0], head_pose[1], head_pose[2], head_pose[3], head_pose[4], head_pose[5], head_pose[6], head_pose[7], head_pose[8]) : "<INVALID>");
        tmp_si_eye.text   = sensor_names[fb.Id] + " Eye: "         + (((valid & hl2da.SI_VALID.EYE)   != 0) ? string.Format("origin=[{0}, {1}, {2}], direction=[{3}, {4}, {5}]", eye_ray[0], eye_ray[1], eye_ray[2], eye_ray[3], eye_ray[4], eye_ray[5]) : "<INVALID>");
        tmp_si_left.text  = sensor_names[fb.Id] + " Left Wrist: "  + (((valid & hl2da.SI_VALID.LEFT)  != 0) ? string.Format("orientation=[{0}, {1}, {2}, {3}], position=[{4}, {5}, {6}], radius={7}, accuracy={8}", left_wrist.rx, left_wrist.ry, left_wrist.rz, left_wrist.rw, left_wrist.tx, left_wrist.ty, left_wrist.tz, left_wrist.radius, left_wrist.accuracy) : "<INVALID>");
        tmp_si_right.text = sensor_names[fb.Id] + " Right Wrist: " + (((valid & hl2da.SI_VALID.RIGHT) != 0) ? string.Format("orientation=[{0}, {1}, {2}, {3}], position=[{4}, {5}, {6}], radius={7}, accuracy={8}", right_wrist.rx, right_wrist.ry, right_wrist.rz, right_wrist.rw, right_wrist.tx, right_wrist.ty, right_wrist.tz, right_wrist.radius, right_wrist.accuracy) : "<INVALID>");
    }

    void Update_ExtendedEyeTracking(hl2da.framebuffer fb)
    {
        // Unpack data
        float[] eye_data = hl2da.user.Unpack1D<float>(fb.Buffer(0), fb.Length(0));
        float[,] pose = hl2da.user.Unpack2D<float>(fb.Buffer(3), hl2da.user.POSE_ROWS, hl2da.user.POSE_COLS);
        hl2da.EE_VALID valid = fb.Valid_EE;

        // Display data
        tmp_ee_combined.text = sensor_names[fb.Id] + " Combined Gaze: " + (((valid & hl2da.EE_VALID.COMBINED_GAZE) != 0) ? string.Format("origin=[{0}, {1}, {2}], direction=[{3}, {4}, {5}], vergence={6}, utc_offset={7}", eye_data[ 0], eye_data[ 1], eye_data[ 2], eye_data[ 3], eye_data[ 4], eye_data[ 5], eye_data[20], utc_offset) : "<INVALID>");
        tmp_ee_left.text     = sensor_names[fb.Id] + " Left Gaze: "     + (((valid & hl2da.EE_VALID.LEFT_GAZE)     != 0) ? string.Format("origin=[{0}, {1}, {2}], direction=[{3}, {4}, {5}], openness={6}", eye_data[ 6], eye_data[ 7], eye_data[ 8], eye_data[ 9], eye_data[10], eye_data[11], eye_data[18]) : "<INVALID>");
        tmp_ee_right.text    = sensor_names[fb.Id] + " Right Gaze: "    + (((valid & hl2da.EE_VALID.RIGHT_GAZE)    != 0) ? string.Format("origin=[{0}, {1}, {2}], direction=[{3}, {4}, {5}], openness={6}", eye_data[12], eye_data[13], eye_data[14], eye_data[15], eye_data[16], eye_data[17], eye_data[19]) : "<INVALID>");
        
        tmp_ee_pose.text = sensor_names[fb.Id] + " Pose: " + PoseToString(pose) + (((valid & hl2da.EE_VALID.CALIBRATION) != 0) ? "" : " <BAD CALIBRATION>");
    }

    void Update_ExtendedAudio(hl2da.framebuffer fb)
    {
        // Get audio format
        hl2da.ea_audioformat format = hl2da.user.Unpack<hl2da.ea_audioformat>(fb.Buffer(2));
        string sample_text = string.Format(": channel_count={0}, bits_per_sample={1}, sample_rate={2}, bitrate={3}, subtype={4}, ", format.channel_count, format.bits_per_sample, format.sample_rate, format.bitrate, format.subtype);
        
        if (format.bits_per_sample == 32)
        {
            float[] samples = hl2da.user.Unpack1D<float>(fb.Buffer(0), fb.Length(0) / sizeof(float));
            sample_text += string.Format("ch0={0}", samples[0]);
        }
        else if (format.bits_per_sample == 16)
        {
            short[] samples = hl2da.user.Unpack1D<short>(fb.Buffer(0), fb.Length(0) / sizeof(short));
            sample_text += string.Format("ch0={0}", samples[0]);
        }
        else
        {
            sample_text += "ch0=<U>";
        }
        
        // Display first sample and format
        tmp_ea.text = sensor_names[fb.Id] + sample_text;
    }

    void Update_ExtendedVideo(hl2da.framebuffer fb)
    {
        // Get video format
        hl2da.ev_videoformat format = hl2da.user.Unpack<hl2da.ev_videoformat>(fb.Buffer(2));
        string format_text = string.Format(": width={0}, height={1}, framerate={2}, subtype={3}", format.width, format.height, format.framerate, format.subtype);

        // Load frame data into textures
        using (hl2da.converter fc = hl2da.converter.Convert(fb.Buffer(0), evcf.width, evcf.height, hl2da.IMT_Format.Yuy2, hl2da.IMT_Format.Bgra8)) // Images from realsense d435i rgb are YUY2
        {
            tex_ev.LoadRawTextureData(fc.Buffer, fc.Length);
            tex_ev.Apply();
        }

        // Display format
        tmp_ev_format.text = sensor_names[fb.Id] + format_text;
    }

    string PoseToString(float[,] pose)
    {
        return string.Format("[[{0}, {1}, {2}, {3}], [{4}, {5}, {6}, {7}], [{8}, {9}, {10}, {11}], [{12}, {13}, {14}, {15}]]", pose[0, 0], pose[0, 1], pose[0, 2], pose[0, 3], pose[1, 0], pose[1, 1], pose[1, 2], pose[1, 3], pose[2, 0], pose[2, 1], pose[2, 2], pose[2, 3], pose[3, 0], pose[3, 1], pose[3, 2], pose[3, 3]);
    }

    string PoseToString(Matrix4x4 pose)
    {
        return string.Format("[[{0}, {1}, {2}, {3}], [{4}, {5}, {6}, {7}], [{8}, {9}, {10}, {11}], [{12}, {13}, {14}, {15}]]", pose[0, 0], pose[0, 1], pose[0, 2], pose[0, 3], pose[1, 0], pose[1, 1], pose[1, 2], pose[1, 3], pose[2, 0], pose[2, 1], pose[2, 2], pose[2, 3], pose[3, 0], pose[3, 1], pose[3, 2], pose[3, 3]);
    }

    void TestPVSettings()
    {
        hl2da.user.PV_SetFocus(hl2da.PV_FocusMode.Manual, hl2da.PV_AutoFocusRange.Normal, hl2da.PV_ManualFocusDistance.Infinity, 200, hl2da.PV_DriverFallback.Disable);
        hl2da.user.PV_SetVideoTemporalDenoising(hl2da.PV_VideoTemporalDenoisingMode.Off);
        hl2da.user.PV_SetWhiteBalance_Preset(hl2da.PV_ColorTemperaturePreset.Manual);
        hl2da.user.PV_SetWhiteBalance_Value(3000);
        hl2da.user.PV_SetExposure(hl2da.PV_ExposureMode.Manual, 2000);
        hl2da.user.PV_SetExposurePriorityVideo(hl2da.PV_ExposurePriorityVideo.Disabled);
        hl2da.user.PV_SetSceneMode(hl2da.PV_CaptureSceneMode.Backlit);
        hl2da.user.PV_SetIsoSpeed(hl2da.PV_IsoSpeedMode.Manual, 100);
        hl2da.user.PV_SetBacklightCompensation(hl2da.PV_BacklightCompensationState.Enable);
        hl2da.user.PV_SetDesiredOptimization(hl2da.PV_MediaCaptureOptimization.LatencyThenPower);
        hl2da.user.PV_SetPrimaryUse(hl2da.PV_CaptureUse.Video);
        hl2da.user.PV_SetOpticalImageStabilization(hl2da.PV_OpticalImageStabilizationMode.On);
        hl2da.user.PV_SetHdrVideo(hl2da.PV_HdrVideoMode.Off);
        hl2da.user.PV_SetRegionsOfInterest(false, true, true, true, true, 0, 0, 1, 1, hl2da.PV_RegionOfInterestType.Unknown, 100);
    }
}
