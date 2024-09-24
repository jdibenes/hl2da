
using System.Runtime.InteropServices;
using UnityEngine;

public class rgbd_align : MonoBehaviour
{
    public GameObject rgb_image;
    public GameObject d_image;
    public int align_algorithm;
    public Texture colormap_texture;
    public Shader colormap_shader;

    private hl2da.pv_captureformat pvcf;

    private float[,] zlt_uv2xy;
    private float[,] zlt_mapxy;
    private float[] zlt_k;
    private Matrix4x4 zlt_extrinsics_inv;
    private Matrix4x4 pv_extrinsics;

    private hl2da.coprocessor.rgbd_align rgbd_aligner;

    private Texture2D tex_rgb;
    private Texture2D tex_d;
    private RenderTexture tex_d_r;
    private Material colormap_mat_lt;

    // Start is called before the first frame update
    void Start()
    {
        pvcf = hl2da.user.CreateFormat_PV(640, 360, 30, false, false);

        hl2da.user.InitializeComponents();
        hl2da.user.OverrideWorldCoordinateSystem();
        hl2da.user.SetFormat_PV(pvcf);
        hl2da.user.SetConstantFactorVLC_RM();
        hl2da.user.BypassDepthLock_RM(true);

        hl2da.user.RM_GetIntrinsics(hl2da.SENSOR_ID.RM_DEPTH_LONGTHROW, out zlt_uv2xy, out zlt_mapxy, out zlt_k);

        float[,] zlt_extrinsics = hl2da.user.RM_GetExtrinsics(hl2da.SENSOR_ID.RM_DEPTH_LONGTHROW);
        using hl2da.pointer p_zlt_extrinsics = hl2da.pointer.get(zlt_extrinsics);
        zlt_extrinsics_inv = Marshal.PtrToStructure<Matrix4x4>(p_zlt_extrinsics.value).inverse;

        pv_extrinsics = Matrix4x4.identity;
        pv_extrinsics[1, 1] = -1;
        pv_extrinsics[2, 2] = -1;

        hl2da.coprocessor.RM_DepthInitializeRays(hl2da.SENSOR_ID.RM_DEPTH_LONGTHROW, zlt_uv2xy);
        rgbd_aligner = hl2da.coprocessor.rgbd_align.create(hl2da.SENSOR_ID.RM_DEPTH_LONGTHROW);

        hl2da.user.Initialize(hl2da.SENSOR_ID.PV, 15);
        hl2da.user.Initialize(hl2da.SENSOR_ID.RM_DEPTH_LONGTHROW, 15);

        hl2da.user.SetEnable(hl2da.SENSOR_ID.RM_DEPTH_LONGTHROW, true);
        hl2da.user.SetEnable(hl2da.SENSOR_ID.PV, true);

        tex_rgb = new Texture2D(pvcf.width, pvcf.height, TextureFormat.BGRA32, false);
        tex_d   = new Texture2D(pvcf.width, pvcf.height, TextureFormat.RFloat, false);

        tex_d_r = new RenderTexture(pvcf.width, pvcf.height, 0, RenderTextureFormat.BGRA32);

        colormap_mat_lt = new Material(colormap_shader);

        colormap_mat_lt.SetTexture("_ColorMapTex", colormap_texture);
        colormap_mat_lt.SetFloat("_Lf", 0.0f);
        colormap_mat_lt.SetFloat("_Rf", 3.0f);

        rgb_image.GetComponent<Renderer>().material.mainTexture = tex_rgb;
        d_image.GetComponent<Renderer>().material.mainTexture = tex_d_r;
    }

    // Update is called once per frame
    void Update()
    {
        using hl2da.framebuffer fb_zlt = hl2da.framebuffer.GetFrame(hl2da.SENSOR_ID.RM_DEPTH_LONGTHROW, -1);
        if (fb_zlt.Status != hl2da.STATUS.OK) { return; }
        
        using hl2da.framebuffer fb_pv = hl2da.framebuffer.GetFrame(hl2da.SENSOR_ID.PV, fb_zlt.Timestamp, hl2da.TIME_PREFERENCE.NEAREST, true);
        if (fb_pv.Status != hl2da.STATUS.OK) { return; }

        hl2da.IMT_ZLTInvalidate(fb_zlt.Buffer(2), fb_zlt.Buffer(0), fb_zlt.Buffer(0));

        Matrix4x4 pose_zlt = Marshal.PtrToStructure<Matrix4x4>(fb_zlt.Buffer(3));
        if (pose_zlt.m33 == 0.0f) { return; }

        Matrix4x4 pose_pv = Marshal.PtrToStructure<Matrix4x4>(fb_pv.Buffer(3));
        if (pose_pv.m33 == 0.0f) { return; }

        Matrix4x4 depth2camera = pv_extrinsics * pose_pv.inverse * pose_zlt * zlt_extrinsics_inv;
        using hl2da.pointer p_depth2camera = hl2da.pointer.get(depth2camera);

        hl2da.pv_metadata metadata = hl2da.user.Unpack<hl2da.pv_metadata>(fb_pv.Buffer(2));
        float[] pv_k = new float[4] { metadata.fx, metadata.fy, metadata.cx, metadata.cy };

        float[,,] pv_z = rgbd_aligner.align(align_algorithm, fb_zlt.Buffer(0), p_depth2camera.value, pv_k, pvcf.width, pvcf.height);
        using hl2da.pointer p_pv_z = hl2da.pointer.get(pv_z);

        using hl2da.converter fc = hl2da.converter.Convert(fb_pv.Buffer(0), pvcf.width, pvcf.height, hl2da.IMT_Format.Nv12, hl2da.IMT_Format.Bgra8); // PV images are NV12

        tex_rgb.LoadRawTextureData(fc.Buffer, fc.Length);
        tex_d.LoadRawTextureData(p_pv_z.value, pvcf.width * pvcf.height * sizeof(float));
        tex_rgb.Apply();
        tex_d.Apply();

        Graphics.Blit(tex_d, tex_d_r, colormap_mat_lt);
    }
}
