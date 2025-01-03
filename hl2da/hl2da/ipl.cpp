
#include "converter.h"
#include "extended_execution.h"
#include "locator.h"
#include "timestamp.h"

#include "extended_audio.h"
#include "extended_eye_tracking.h"
#include "extended_video.h"
#include "microphone_capture.h"
#include "personal_video.h"
#include "research_mode.h"
#include "spatial_input.h"

#include "stream_ea.h"
#include "stream_ee.h"
#include "stream_ev.h"
#include "stream_mc.h"
#include "stream_pv.h"
#include "stream_rm.h"
#include "stream_si.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
void HL2DA_Load()
{
    Converter_Initialize();
    ExtendedExecution_Initialize();
    Locator_Initialize();
    Timestamp_Initialize();

    ResearchMode_Startup();
    PersonalVideo_Startup();
    MicrophoneCapture_Startup();
    SpatialInput_Startup();
    // - EV
    // - EA
    // - EET

    RM_InitializeDepthLock();
}
