/*

Here are the ugly macros from C for when we want them:

DECLARE_INTERFACE_(IDirectSound, IUnknown)
{
    // IUnknown methods
    STDMETHOD(QueryInterface)       (THIS_ __in REFIID, __deref_out LPVOID*) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    // IDirectSound methods
    STDMETHOD(CreateSoundBuffer)    (THIS_ __in LPCDSBUFFERDESC pcDSBufferDesc, __deref_out LPDIRECTSOUNDBUFFER *ppDSBuffer, __null LPUNKNOWN pUnkOuter) PURE;
    STDMETHOD(GetCaps)              (THIS_ __out LPDSCAPS pDSCaps) PURE;
    STDMETHOD(DuplicateSoundBuffer) (THIS_ __in LPDIRECTSOUNDBUFFER pDSBufferOriginal, __deref_out LPDIRECTSOUNDBUFFER *ppDSBufferDuplicate) PURE;
    STDMETHOD(SetCooperativeLevel)  (THIS_ HWND hwnd, DWORD dwLevel) PURE;
    STDMETHOD(Compact)              (THIS) PURE;
    STDMETHOD(GetSpeakerConfig)     (THIS_ __out LPDWORD pdwSpeakerConfig) PURE;
    STDMETHOD(SetSpeakerConfig)     (THIS_ DWORD dwSpeakerConfig) PURE;
    STDMETHOD(Initialize)           (THIS_ __in_opt LPCGUID pcGuidDevice) PURE;
};

*/




OUTPUT_SAMPLING_RATE :: 48000;

PRIMARY_BUFFER_SIZE_IN_BYTES :: 0;
FILL_BUFFER_SIZE_IN_SAMPLES  :: 24000;  // @Cleanup: When we can do constexpr math, this should be: OUTPUT_SAMPLING_RATE / 2 or something like that.

HWND :: HANDLE;

WAVE_FORMAT_PCM   :: 0x0001;
WAVE_FORMAT_ADPCM :: 0x0002;

DS_OK :: 0;


IDirectSound8 :: struct {
    vtable: *IDirectSound8Vtable;
}

IDirectSound8Vtable :: struct {
    QueryInterface: *void;
    AddRef: *void;
    Release: *void;

    // IDirectSound methods
    CreateSoundBuffer: *void;
    GetCaps: *void;
    DuplicateSoundBuffer: *void;
    SetCooperativeLevel: (dsound: *IDirectSound8, hwnd: HWND, level: s32) -> s32 #foreign;
    Compact: *void;
    GetSpeakerConfig: *void;
    SetSpeakerConfig: *void;
    Initialize: *void;
}

IDirectSoundBuffer8 :: struct {}

Sound_Handler :: struct {
    playing: bool;

    fill_buffer_size_in_bytes: s32;
    minimum_prebuffered_bytes: s32;

    num_channels:  s32;  // Currently will only be 2 or 6.
    channel_names: [..] string;

    my_write_cursor: s64;

    dsound:         *IDirectSound8;
    primary_buffer: *IDirectSoundBuffer8;
    fill_buffer:    *IDirectSoundBuffer8;
};

init_wave_format :: (using w: Waveformatex, nchannels: s16) {
    wFormatTag      = WAVE_FORMAT_PCM; 
    nChannels       = nchannels;
    nSamplesPerSec  = OUTPUT_SAMPLING_RATE; 
    wBitsPerSample  = 16;
    nBlockAlign     = (wBitsPerSample / 8) * nChannels;
    nAvgBytesPerSec = nSamplesPerSec * nBlockAlign;
}

init_sound_handler :: (using handler: *Sound_Handler, hwnd: HWND) -> bool {
    DirectSoundCreate8 :: (device: *void, ppds8: **IDirectSound8, outer: *void) -> s32 #foreign;

    hr := DirectSoundCreate8(null, *dsound, null);
    print("hr is %\n", hr);
    if hr != DS_OK return false;

    print("hr created\n");

    DSSCL_NORMAL        :: 0x1;
    DSSCL_PRIORITY      :: 0x2;
    // DSSCL_EXCLUSIVE     :: 0x3;   Deprecated and useless.
    // DSSCL_WRITEPRIMARY  :: 0x4;   Deprecated and useless.
    
    vt := dsound.vtable;

    print("CSB is %\n", vt.CreateSoundBuffer);
    print("GetCaps is %\n", vt.GetCaps);
    print("DSB is %\n", vt.DuplicateSoundBuffer);
    print("SCL is %\n", vt.SetCooperativeLevel);
    print("QI is %\n", vt.QueryInterface);
    print("AR is %\n", vt.AddRef);
    print("Compact is %\n", vt.Compact);

    print("hwnd is %\n", hwnd);

    print("vt.SCL is %\n", vt.SetCooperativeLevel);

    hr = vt.SetCooperativeLevel(dsound, hwnd, DSSCL_PRIORITY);
    print("hr is %\n", hr);
    
/*
    hr = dsound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
    if !hr return false;
*/
    
/*
    DWORD speaker_config;
    hr = dsound->GetSpeakerConfig(&speaker_config);
    DWORD config, geometry;
    if (SUCCEEDED(hr)) {
        config = DSSPEAKER_CONFIG(speaker_config);
        geometry = DSSPEAKER_GEOMETRY(speaker_config);

        switch (config) {
        case DSSPEAKER_5POINT1_BACK:
        case DSSPEAKER_5POINT1_SURROUND:
        case DSSPEAKER_7POINT1_WIDE:
        case DSSPEAKER_7POINT1_SURROUND:
            num_channels = 6;

            array_add(*channel_names, "Front Left");
            array_add(*channel_names, "Front Right");
            array_add(*channel_names, "Center");
            array_add(*channel_names, "Subwoofer");
            array_add(*channel_names, "Rear Left");
            array_add(*channel_names, "Rear Right");

			break;
        default:
            num_channels = 2;

            array_add(*channel_names, "Left");
            array_add(*channel_names, "Right");
			break;
        }
    } else {
        config = (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT);
        array_add(*channel_names, "Left?");
        array_add(*channel_names, "Right?");
        num_channels = 2;
    }


    //
    // Get the primary buffer 
    //
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = PRIMARY_BUFFER_SIZE_IN_BYTES;
    dsbd.lpwfxFormat   = NULL;
    
    hr = dsound->CreateSoundBuffer(&dsbd, (IDirectSoundBuffer **)&primary_buffer, NULL);
    assert(SUCCEEDED(hr));
    if (FAILED (hr)) return false;

    DWORD channel_mask;
    if (num_channels == 2) {
        channel_mask = KSAUDIO_SPEAKER_STEREO;
    } else {
        channel_mask = KSAUDIO_SPEAKER_5POINT1_SURROUND;
    }

    WAVEFORMATEXTENSIBLE wfx;
    memset(&wfx, 0, sizeof(wfx));
    init_wave_format(&wfx.Format, num_channels);
    wfx.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    wfx.Format.wBitsPerSample = 16;
    wfx.Format.nSamplesPerSec = OUTPUT_SAMPLING_RATE;
    wfx.Format.cbSize = 22;
    wfx.Samples.wValidBitsPerSample = 16;
    wfx.dwChannelMask = channel_mask;
    wfx.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
    hr = primary_buffer->SetFormat(&wfx.Format);
    assert(SUCCEEDED(hr));
    if (FAILED(hr)) return false;


    WAVEFORMATEXTENSIBLE wfx2;
    memset(&wfx2, 0, sizeof(wfx2));
    wfx2.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    wfx2.Format.cbSize = 22;

    DWORD needed = 0;
    hr = primary_buffer->GetFormat(NULL, 0, &needed);
    char *result_bytes = new char[needed];
    hr = primary_buffer->GetFormat((LPWAVEFORMATEX)result_bytes, needed, NULL);
    assert(SUCCEEDED(hr));

    //
    // @Incomplete: What if our result format has a different number of channels from what we wanted?
    //

    int fill_bytes = FILL_BUFFER_SIZE_IN_SAMPLES * num_channels;

    //
    // Create the fill buffer.
    //
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2;
    dsbd.dwBufferBytes = fill_bytes;
    dsbd.lpwfxFormat   = &wfx.Format;

    hr = dsound->CreateSoundBuffer(&dsbd, (IDirectSoundBuffer **)&fill_buffer, NULL);
    assert(SUCCEEDED(hr));
    if (FAILED(hr)) return false;


    fill_buffer_size_in_bytes = fill_bytes;  // @Robustness: Check the actual length of the buffer, not what we asked it to be.
    return true;
*/
}
/*
void Sound_Handler::play() {
    if (!fill_buffer) return; // No audio driver!

    HRESULT hr = fill_buffer->Play(0, 0, DSBPLAY_LOOPING);

    assert(SUCCEEDED(hr));
    if (SUCCEEDED(hr)) playing = true;

    return;
}

static void add_result(Fill_Region_Result *result, void *pointer, DWORD bytes) {
    int index = result->num_regions;
    result->buffer_pointer[index] = pointer;
    result->length_in_bytes[index] = bytes;

    result->num_regions++;
}

static void unpack_result(Fill_Region_Result *result, int index,
                          void **pointer_return, DWORD *bytes_return) {
    if (index >= result->num_regions) {
        *pointer_return = NULL;
        *bytes_return = 0;
    }

    *pointer_return = result->buffer_pointer[index];
    *bytes_return = result->length_in_bytes[index];
}


void Sound_Handler::get_fill_regions(int bytes_to_lock, Fill_Region_Result *result) {
    result->num_regions = 0;
	if (!fill_buffer) return;  // No driver, no regions!

    HRESULT hr;
    void *pointer1;
    DWORD bytes1;
    void *pointer2;
    DWORD bytes2;
    
    hr = fill_buffer->Lock(my_write_cursor, bytes_to_lock, 
                           &pointer1, &bytes1, &pointer2, &bytes2, 
                           0);
    assert(!FAILED(hr));
    if (FAILED(hr)) return;

    add_result(result, pointer1, bytes1);
    add_result(result, pointer2, bytes2);
}

void Sound_Handler::release_fill_regions(Fill_Region_Result *result) {
    if (!fill_buffer) return;  // No driver!
	
	void *pointer1, *pointer2;
    DWORD bytes1, bytes2;

    unpack_result(result, 0, &pointer1, &bytes1);
    unpack_result(result, 1, &pointer2, &bytes2);

    fill_buffer->Unlock(pointer1, bytes1, pointer2, bytes2);

    if (pointer1) my_write_cursor += bytes1;
    if (pointer2) my_write_cursor += bytes2;

    if (my_write_cursor >= fill_buffer_size_in_bytes) my_write_cursor -= fill_buffer_size_in_bytes;

    assert(my_write_cursor >= 0);
    assert(my_write_cursor < fill_buffer_size_in_bytes);
    assert((my_write_cursor % 4) == 0);
}

bool Sound_Handler::needs_async_update_from_main_thread() {
    return true;
}

void Sound_Handler::shutdown() {
    if (fill_buffer) {
        fill_buffer->Stop();
    }

    SAFE_RELEASE(primary_buffer);
    SAFE_RELEASE(fill_buffer);
    SAFE_RELEASE(dsound); 
}

static bool restore_buffer(IDirectSoundBuffer8 *buffer) {
    HRESULT hr;
    DWORD dwStatus;
    hr = buffer->GetStatus(&dwStatus);
    if (FAILED(hr)) return false;

    if (!(dwStatus & DSBSTATUS_BUFFERLOST)) return true;

    hr = buffer->Restore();
    if (FAILED(hr)) return false;

    return true;
}

bool Sound_Handler::restore_buffers() {
    bool success1 = restore_buffer(primary_buffer);
    bool success2 = restore_buffer(fill_buffer);

    return success1 && success2;
}

static int get_bytes_between(int front, int rear, int buffer_len) {
    if (front >= rear) return front - rear;
    return (buffer_len - rear) + front;
}

int Sound_Handler::count_buffered_bytes() {
	if (!fill_buffer) return 0;

//    restore_buffers();

    DWORD d_play_cursor;
    DWORD d_annoying_write_cursor;

    HRESULT hr = fill_buffer->GetCurrentPosition(&d_play_cursor, &d_annoying_write_cursor);

//    assert(SUCCEEDED(hr));
    if (!SUCCEEDED(hr)) return 0;
    
    int between = get_bytes_between(my_write_cursor, d_play_cursor, fill_buffer_size_in_bytes);

    int necessary_prebuffered = get_bytes_between(d_annoying_write_cursor, d_play_cursor, fill_buffer_size_in_bytes);
//    if (minimum_prebuffered_bytes < necessary_prebuffered) minimum_prebuffered_bytes = necessary_prebuffered;
    minimum_prebuffered_bytes = necessary_prebuffered;

//    xxx_s2 = necessary_prebuffered;

    return between;
}
*/
