
using UnityEngine;

public class test_pv : MonoBehaviour
{
    public GameObject pv_image;
    public ushort pv_width;
    public ushort pv_height;
    public byte pv_fps;

    private hl2da.pv_captureformat pvcf;

    private Texture2D tex_pv;

    // Start is called before the first frame update
    void Start()
    {
        pvcf = hl2da.user.CreateFormat_PV(pv_width, pv_height, pv_fps, false, false);

        hl2da.user.InitializeComponents();
        hl2da.user.OverrideWorldCoordinateSystem();
        hl2da.user.SetFormat_PV(pvcf);

        hl2da.user.Initialize(hl2da.SENSOR_ID.PV, 15); // Max 18
        hl2da.user.SetEnable(hl2da.SENSOR_ID.PV, true);

        tex_pv = new Texture2D(pvcf.width, pvcf.height, TextureFormat.BGRA32, false);
        pv_image.GetComponent<Renderer>().material.mainTexture = tex_pv;
    }

    // Update is called once per frame
    void Update()
    {
        using var fb = hl2da.framebuffer.GetFrame(hl2da.SENSOR_ID.PV, -1);
        if (fb.Status != hl2da.STATUS.OK) { return; }

        uint stride = hl2da.converter.GetStride_PV(pvcf.width);

        using hl2da.converter fc = hl2da.converter.Convert(fb.Buffer(0), stride, pvcf.height, hl2da.IMT_Format.Nv12, hl2da.IMT_Format.Bgra8);

        if (stride != pvcf.width)
        {
            byte[,,] image = hl2da.coprocessor.Crop<byte>(fc.Buffer, (int)stride, pvcf.height, 4, 0, 0, pvcf.width, pvcf.height);
            using hl2da.pointer p = hl2da.pointer.get(image);
            tex_pv.LoadRawTextureData(p.value, pvcf.width * pvcf.height * 4);
        }
        else
        {
            tex_pv.LoadRawTextureData(fc.Buffer, fc.Length);
        }
                
        tex_pv.Apply();
    }
}
