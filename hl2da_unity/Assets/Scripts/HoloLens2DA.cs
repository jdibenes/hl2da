using System.Collections;
using System.Collections.Generic;
using UnityEngine;

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

        hl2da.InitializeStream(0, 300);
        hl2da.InitializeStream(1, 300);
        hl2da.InitializeStream(2, 300);
        hl2da.InitializeStream(3, 300);
        hl2da.InitializeStream(4, 450);
        hl2da.InitializeStream(5, 5); // must be small

        hl2da.SetStreamEnable(0, true);
        hl2da.SetStreamEnable(1, true);
        hl2da.SetStreamEnable(2, true);
        hl2da.SetStreamEnable(3, true);
        //hl2da.SetStreamEnable(4, true);
        hl2da.SetStreamEnable(5, true);

        tex_lf = new Texture2D(640, 480, TextureFormat.R8, false);
        tex_ll = new Texture2D(640, 480, TextureFormat.R8, false);
        tex_rf = new Texture2D(640, 480, TextureFormat.R8, false);
        tex_rr = new Texture2D(640, 480, TextureFormat.R8, false);
        tex_ht = new Texture2D(512, 512, TextureFormat.R16, false);
        tex_ht_ab = new Texture2D(512, 512, TextureFormat.R16, false);
        tex_lt = new Texture2D(320, 288, TextureFormat.R16, false);
        tex_lt_ab = new Texture2D(320, 288, TextureFormat.R16, false);
        tex_lt_sigma = new Texture2D(320, 288, TextureFormat.R8, false);
    }

    // Update is called once per frame
    void Update()
    {
        hl2da.frame_buffer fb;
        fb = hl2da.GetStreamFrame(0, -1);
        if (fb.status == 0)
        {
            tex_lf.LoadRawTextureData(fb.buffer, fb.length);
            tex_lf.Apply();
            quad_lf.GetComponent<Renderer>().material.mainTexture = tex_lf;
        }
        fb = hl2da.GetStreamFrame(1, -1);
        if (fb.status == 0)
        {
            tex_ll.LoadRawTextureData(fb.buffer, fb.length);
            tex_ll.Apply();
            quad_ll.GetComponent<Renderer>().material.mainTexture = tex_ll;
        }
        fb = hl2da.GetStreamFrame(2, -1);
        if (fb.status == 0)
        {
            tex_rf.LoadRawTextureData(fb.buffer, fb.length);
            tex_rf.Apply();
            quad_rf.GetComponent<Renderer>().material.mainTexture = tex_rf;
        }
        fb = hl2da.GetStreamFrame(3, -1);
        if (fb.status == 0)
        {
            tex_rr.LoadRawTextureData(fb.buffer, fb.length);
            tex_rr.Apply();
            quad_rr.GetComponent<Renderer>().material.mainTexture = tex_rr;
        }
        /*
        fb = hl2da.GetStreamFrame(4, -1);
        if (fb.status == 0)
        {
            tex_ht.LoadRawTextureData(fb.buffer, fb.length * 2);
            tex_ht.Apply();
            tex_ht_ab.LoadRawTextureData(fb.ab_depth_buffer, fb.ab_depth_length * 2);
            tex_ht_ab.Apply();
            quad_ht.GetComponent<Renderer>().material.mainTexture = tex_ht;
            quad_ht_ab.GetComponent<Renderer>().material.mainTexture = tex_ht_ab;
        }
        */
        fb = hl2da.GetStreamFrame(5, -1);
        if (fb.status == 0)
        {
            tex_lt.LoadRawTextureData(fb.buffer, fb.length * 2);
            tex_lt.Apply();
            tex_lt_ab.LoadRawTextureData(fb.ab_depth_buffer, fb.ab_depth_length * 2);
            tex_lt_ab.Apply();
            tex_lt_sigma.LoadRawTextureData(fb.sigma_buffer, fb.sigma_length);
            tex_lt_sigma.Apply();
            quad_lt.GetComponent<Renderer>().material.mainTexture = tex_lt;
            quad_lt_ab.GetComponent<Renderer>().material.mainTexture = tex_lt_ab;
            quad_lt_sigma.GetComponent<Renderer>().material.mainTexture = tex_lt_sigma;
            //fb.Destroy();
        }
    }
}
