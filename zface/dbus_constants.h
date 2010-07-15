#ifndef DBUS_CONSTANTS_H_
#define DBUS_CONSTANTS_H_

//---------------------------------------------------------------------

#define DBUS_PREFIX         "com.speechpro."
#define DBUS_ROOT_PATH      "/"
#define DBUS_ALIVE_PATH     DBUS_ROOT_PATH "alive"

//---------------------------------------------------------------------

// --- широковещательные интерфейсы ---
#define DBUS_I_ENCODERS_EVENTS          DBUS_PREFIX "EncodersEvents"
#define DBUS_I_RECORD_KEY_EVENTS        DBUS_PREFIX "RecordKeyEvents"
#define DBUS_I_CURSOR_KEYS_EVENTS       DBUS_PREFIX "CursorKeysEvents"
#define DBUS_I_TIMER_RECORD_REQUESTS    DBUS_PREFIX "TimerRecRequests"
#define DBUS_I_PARAM_CHANGED_EVENTS     DBUS_PREFIX "ParamChangedEvents"
#define DBUS_I_PARAM_CHANGE_REQUESTS    DBUS_PREFIX "ParamChangeRequests"

#define DBUS_I_RECORDER_CONTROL         DBUS_PREFIX "RecControl"
#define DBUS_I_RECORDER_DURATION        DBUS_PREFIX "RecDurationEvents"
#define DBUS_I_PLAYER_LEVEL             DBUS_PREFIX "PlayLevelEvents"
#define DBUS_I_PLAYER_CONTROL           DBUS_PREFIX "PlayControl"
#define DBUS_I_PLAYER_STATE             DBUS_PREFIX "PlayStateEvents"
#define DBUS_I_PLAYER_POSITION          DBUS_PREFIX "PlayPositionEvents"

#define DBUS_I_PLAYER_LEVEL             DBUS_PREFIX "PlayLevelEvents"
#define DBUS_I_HEART_BEATS              DBUS_PREFIX "HeartBeats"
#define DBUS_I_GAIN_CHANGED_EVENTS      DBUS_PREFIX "GainChangedEvents"

#define DBUS_I_PARAM_REQUESTS           DBUS_PREFIX "ParamsRequests"

#define DBUS_I_MESSAGE_FOR_USER         DBUS_PREFIX "MessageForUser"

#define DBUS_I_PRESETS                  DBUS_PREFIX "Presets"

//---------------------------------------------------------------------

// --- интефейсы конкретных служб ---
#define DBUS_N_ZVAR                     DBUS_PREFIX "zvar"
#define DBUS_N_ZTIME                    DBUS_PREFIX "ztime"
#define DBUS_N_ZMONSTER                 DBUS_PREFIX "zmonster"
#define DBUS_N_ZSTREAMER                DBUS_PREFIX "zstreamer"
#define DBUS_N_ZKEY                     DBUS_PREFIX "zkey"
#define DBUS_N_ZFACE                    DBUS_PREFIX "zface"
#define DBUS_N_ZPLAY                    DBUS_PREFIX "zplay"
#define DBUS_N_ZDENOISER                DBUS_PREFIX "zdenoiser"
#define DBUS_N_ZSPACE                   DBUS_PREFIX "zspace"
#define DBUS_N_ZMIX                     DBUS_PREFIX "zmix"
#define DBUS_N_ZDOG                     DBUS_PREFIX "zdog"

//---------------------------------------------------------------------

// --- методы ---
#define DBUS_M_REQUEST_PARAM            "RequestParam"
#define DBUS_M_OPEN_FILE                "OpenFile"

// --- пресеты ---
#define DBUS_M_SAVE_AS_CURRENT          "SaveAsCurrent"
#define DBUS_M_SAVE_CURRENT_AS          "SaveCurrentAs"
#define DBUS_M_DELETE                   "Delete"
#define DBUS_M_RENAME                   "Rename"
#define DBUS_M_LISTING                  "Listing"

//---------------------------------------------------------------------

// --- команда на изменение параметра и уведомление об изменении ---
#define DBUS_M_CHANGE_PARAM             "ChangeParam"
#define DBUS_S_PARAM_CHANGED            "ParamChanged"

// --- уведомления о вращении "крутилок" ---
#define DBUS_S_PLAY_ENCODER             "PlayEncoder"
#define DBUS_S_LREC_ENCODER             "LeftRecEncoder"
#define DBUS_S_RREC_ENCODER             "RightRecEncoder"

// --- команды управления записью ---
#define DBUS_S_START_RECORD             "StartRec"
#define DBUS_S_STOP_RECORD              "StopRec"
#define DBUS_S_TIMER_START_REC          "StartRec"
#define DBUS_S_TIMER_STOP_REC           "StopRec"
#define DBUS_S_FORCE_STOP_REC           "ForceStopRec"
#define DBUS_S_QUIT_RECORDER            "Quit"

// --- уведомления о состоянии записи ---
#define DBUS_S_REC_DURATION             "RecDuration"

// --- команды управления воспроизведением ---
#define DBUS_S_START_PLAY               "StartPlay"
#define DBUS_S_STOP_PLAY                "StopPlay"
#define DBUS_S_PAUSE_PLAY               "PausePlay"
#define DBUS_S_REWIND_PLAY              "Rewind"
#define DBUS_S_FAST_FORWARD_PLAY        "FastForward"

// --- уведомления о состоянии воспроизведения ---
#define DBUS_S_PLAY_LEVEL               "PlayLevel"
#define DBUS_S_PLAY_POSITION            "PositionChanged"

#define DBUS_S_PLAY_STARTED             "Playing"
#define DBUS_S_PLAY_STOPPED             "Stopped"
#define DBUS_S_PLAY_PAUSED              "Paused"

// --- управление фоновой шумоочисткой ---
#define DBUS_S_START_CLEAN              "StartClean"
#define DBUS_S_STOP_CLEAN               "StopClean"

// --- уведомления о нажатии кнопок ---
#define DBUS_S_RECORD_KEY               "RecordKey"
#define DBUS_S_LEFT_KEY                 "LeftKey"
#define DBUS_S_RIGHT_KEY                "RightKey"
#define DBUS_S_UP_KEY                   "UpKey"
#define DBUS_S_DOWN_KEY                 "DownKey"
#define DBUS_S_ENTER_KEY                "EnterKey"
#define DBUS_S_ESC_KEY                  "EscapeKey"

#define DBUS_S_REFRESH_DISPLAY          "RefreshDisplay"

// ---  ---
#define DBUS_S_GAIN_CHANGED             "GainChanged"

// --- сообщения для пользователя ---
#define DBUS_S_ERROR                    "Error"
#define DBUS_S_NOTIFY                   "Notify"


//---------------------------------------------------------------------

#define DBUS_S_ZMIX_ALIVE               "zmixAlive"
#define DBUS_S_ZREC_ALIVE               "zrecAlive"
#define DBUS_S_ZPLAY_ALIVE              "zplayAlive"
#define DBUS_S_ZFACE_ALIVE              "zfaceAlive"
#define DBUS_S_ZKEY_ALIVE               "zkeyAlive"
#define DBUS_S_ZTIME_ALIVE              "ztimeAlive"
#define DBUS_S_ZVAR_ALIVE               "zvarAlive"
#define DBUS_S_ZSTREAMER_ALIVE          "zstreamerAlive"
#define DBUS_S_ZSPACE_ALIVE             "zspaceAlive"
#define DBUS_S_ZMONSTER_ALIVE           "zmonsterAlive"
#define DBUS_S_ZDENOISER_ALIVE          "zdenoiserAlive"

//---------------------------------------------------------------------

#define SECTION_REGULAR                 "Main"
#define SECTION_TEMPORARY               "Temp"
#define SECTION_FILTERS                 "Filters"

#define PRESET_CURRENT                  "Current"
#define PRESET_DEFAULT                  "Default"

// regular params
#define PARAM_MIXER_INPUT                      "Mixer.Input"
#define PARAM_MIXER_OUTPUT                     "Mixer.Output"
#define PARAM_MIXER_CHANNELS_TO_SPEAKER        "Mixer.Channels_to_speaker"
#define PARAM_MIXER_INPUT_MIC_LEFT_GAIN        "Mixer.Input.Mic.Left_gain"
#define PARAM_MIXER_INPUT_MIC_RIGHT_GAIN       "Mixer.Input.Mic.Right_gain"
#define PARAM_MIXER_INPUT_ANALOG_LEFT_GAIN     "Mixer.Input.Analog.Left_gain"
#define PARAM_MIXER_INPUT_ANALOG_RIGHT_GAIN    "Mixer.Input.Analog.Right_gain"
#define PARAM_MIXER_INPUT_ANALOG_MODE          "Mixer.Input.Analog.Mode"
#define PARAM_MIXER_INPUT_ANALOG_SIGNAL_KIND   "Mixer.Input.Analog.Signal_kind"
#define PARAM_MIXER_INPUT_PHONE_GAIN           "Mixer.Input.Phone.Gain"
#define PARAM_MIXER_INPUT_DIGITAL_LEFT_GAIN    "Mixer.Input.DigitalIn.Left_gain"
#define PARAM_MIXER_INPUT_DIGITAL_RIGHT_GAIN   "Mixer.Input.DigitalIn.Right_gain"
#define PARAM_MIXER_OUTPUT_LINE_GAIN           "Mixer.Output.LineOut.Gain"
#define PARAM_MIXER_OUTPUT_SPEAKER_GAIN        "Mixer.Output.Speaker.Gain"
#define PARAM_MIXER_OUTPUT_DIGITAL_GAIN        "Mixer.Output.DigitalOut.Gain"
#define PARAM_MIXER_THROUGH_CHANNEL_GAIN       "Mixer.Through_channel.Gain"
#define PARAM_RECORDER_SAMPLE_RATE             "Recorder.Sample_rate"
#define PARAM_RECORDER_PRECISION               "Recorder.Sample_size"
#define PARAM_RECORDER_COMPRESSION             "Recorder.Compression"
#define PARAM_RECORDER_RECORD_DURATION_LIMIT   "Recorder.Record_duration_limit"
#define PARAM_RECORDER_CHANNELS                "Recorder.Channels"
#define PARAM_RECORDER_RECORD_FILES            "Recorder.Record_files"
#define PARAM_RECORDER_AGC_STATE               "Recorder.AGC.Enabled"
#define PARAM_RECORDER_AGC_ATTACK              "Recorder.AGC.Attack_time"
#define PARAM_RECORDER_AGC_DECAY               "Recorder.AGC.Decay_time"
#define PARAM_RECORDER_AGC_NOISE               "Recorder.AGC.Noise_threshold"
#define PARAM_RECORDER_AGC_GAIN                "Recorder.AGC.Target_gain"
#define PARAM_RECORDER_TIMER_STATE             "Recorder.Timer.Enabled"
#define PARAM_RECORDER_TIMER_START             "Recorder.Timer.Start_time"
#define PARAM_RECORDER_TIMER_DURATION          "Recorder.Timer.Duration"
#define PARAM_RECORDER_TIMER_REPEAT            "Recorder.Timer.Repeat"
#define PARAM_RECORDER_ACOUSTIC_STATE          "Recorder.Acoustic.Enabled"
#define PARAM_RECORDER_ACOUSTIC_START          "Recorder.Acoustic.Start_threshold"
#define PARAM_RECORDER_ACOUSTIC_STARTDELAY     "Recorder.Acoustic.Start_delay"
#define PARAM_RECORDER_ACOUSTIC_STOP           "Recorder.Acoustic.Stop_threshold"
#define PARAM_RECORDER_ACOUSTIC_STOPDELAY      "Recorder.Acoustic.Stop_delay"
#define PARAM_RECORDER_ENABLE_LEDS             "Recorder.Enable_LEDs"
#define PARAM_PLAYER_AGC_STATE                 "Player.AGC.Enabled"
#define PARAM_PLAYER_AGC_ATTENUATION           "Player.AGC.Attenuation_gain"
#define PARAM_PLAYER_AGC_NOISE                 "Player.AGC.Noise_gain"
#define PARAM_PLAYER_AGC_DELAY                 "Player.AGC.Delay"
#define PARAM_PLAYER_NOISE_CANCELATION         "Player.Noise_cancelation"
#define PARAM_OWERFLOW_GUARD_CLEAN_TRASH       "Overflow_guard.Auto_clean_trash"
#define PARAM_OWERFLOW_GUARD_DELETE_OLD        "Overflow_guard.Auto_delete_old"
#define PARAM_DISPLAY_BACKLIGHT_AUTO_OFF       "Display.Backlight.Auto_turn_off"
#define PARAM_USB_CONNECTED_OS                 "USB.Connected_OS"
#define PARAM_SERIAL_NUMBER                    "Serial_number"
#define PARAM_SECURITY_KEYBOARD_LOCK           "Security.Keyboard_lock.Auto_lock"
#define PARAM_SECURITY_PROTECTION_ENABLED      "Security.Protection.Enabled"
#define PARAM_SECURITY_PROTECTION_PIN          "Security.Protection.PIN"

// filters
#define PARAM_BROADBAND_FILTER_ENABLED          "Broadband_filter.Enabled"
#define PARAM_BROADBAND_FILTER_SUPRESSION       "Broadband_filter.Supression"
#define PARAM_BROADBAND_FILTER_LOW_BAND_NOISE   "Broadband_filter.LowBandNoise"
#define PARAM_BROADBAND_FILTER_REVERBERATION    "Broadband_filter.Reverberation"
#define PARAM_BROADBAND_FILTER_RESIDUAL         "Broadband_filter.Residual"
#define PARAM_HARMONIC_REJECT_FILTER_ENABLED    "HarmonicReject_filter.Enabled"
#define PARAM_EQ_FILTER_ENABLED                 "EQ_filter.Enabled"
#define PARAM_EQ_FILTER_TYPE                    "EQ_filter.Type"
#define PARAM_EQ_FILTER_LF                      "EQ_filter.LF"
#define PARAM_EQ_FILTER_HF                      "EQ_filter.HF"
#define PARAM_DECLICKER_FILTER_ENABLED          "Declicker_filter.Enabled"
#define PARAM_DECLICKER_FILTER_THRESHOLD        "Declicker_filter.Threshold"
#define PARAM_LEVEL_ENHANCEMENT_FILTER_ENABLED  "LevelEnhancement_filter.Enabled"
#define PARAM_REFERENCE_NOISE_FILTER_ENABLED             "ReferenceNoise_filter.Enabled"
#define PARAM_REFERENCE_NOISE_FILTER_REFERENCE_CHANNEL   "ReferenceNoise_filter.ReferenceChannel"
#define PARAM_REFERENCE_NOISE_FILTER_TIME_DELAY          "ReferenceNoise_filter.TimeDelay"
#define PARAM_REFERENCE_NOISE_FILTER_SUPRESSION          "ReferenceNoise_filter.Supression"
#define PARAM_PSEUDO_FILTER_ENABLED             "Pseudo_filter.Enabled"
#define PARAM_PSEUDO_FILTER_DELAY               "Pseudo_filter.Delay"
#define PARAM_VOICE_CHANGER_FILTER_ENABLED      "Voice_changer_filter.Enabled"
#define PARAM_VOICE_CHANGER_FILTER_VOICE        "Voice_changer_filter.Voice"

// temporary params
#define PARAM_STORAGE_CONNECTED                "Storage.Connected"
#define PARAM_STORAGE_FORMATTED                "Storage.Formatted"
#define PARAM_STORAGE_CAPACITY                 "Storage.Capacity"
#define PARAM_STORAGE_FREE_SPACE               "Storage.Free_space"
#define PARAM_MIXER_TUMBLER_SIGNAL_KIND        "Mixer.Tumbler.Signal_kind"
#define PARAM_MIXER_TUMBLER_INPUT_MODE         "Mixer.Tumbler.Input_mode"
#define PARAM_MIXER_HEADSET_CONNECTED          "Mixer.Headset.Connected"
#define PARAM_MIXER_THROUGH_CHANNEL            "Mixer.Through_channel"
#define PARAM_MIXER_WEB_THROUGH_CHANNEL        "Mixer.Web_Through_channel"
#define PARAM_MONITORING_ENABLED               "Monitoring.Enabled"
#define PARAM_SECURITY_KEYBOARD_LOCK_ACTIVE    "Security.Keyboard_lock.Active"
#define PARAM_POWER_BATTERY_CHARGE             "Power.Battery.Charge"
#define PARAM_POWER_SOURCE                     "Power.Source"
#define PARAM_TIMER_WANTS_REC                  "Timer.Wants_rec"
#define PARAM_RECORDER_WANTS_REC               "Recorder.Wants_rec"
#define PARAM_RECORDER_STATE                   "Recorder.State"

//---------------------------------------------------------------------

#define ZDOG_SEND_DELAY (time_t)5
#define DBUS_REINIT_DELAY (time_t)5

//---------------------------------------------------------------------

// Константы для сообщений от служб пользователю
enum
{
    MSG_NO_SPACE=1,                     // Error:  нет места на карте памяти
    MSG_RECORDING_PARAM_DELAY,          // Notify: параметры записи будут применены к следующей записи, так как сейчас идёт запись
    MSG_THROUGH_CHANNEL_PARAM_DELAY,    // Notify: параметры сквозняков будут применены к следующему страту
    MSG_NO_PERM,                        // Error:  нет прав на файл (например read only filesystem)
    MSG_RECORDING_CHANGED_INPUT,        // Notify: "остановили запись так как поменялся вход"
    MSG_THROUGH_CHANNEL_OFF,            // Notify: "отключаем сквозной канал"
    MSG_MMC_BUSY,                       // Notify: "карта памяти используется"
    MSG_FORMATTING_ERROR,               // Error: "ошибка форматирования"
    MSG_FORMATTING_COMPLETE,            // Notify: "форматирование успешно завершено"
    MSG_CANT_ENCODE_MP3,                // Error: "не могу начать кодирование MP3"
    MSG_RECORDING_IN_PROGRESS,          // Notify: "идёт запись, воспроизведение невозможно"
    MSG_RECORDING_MMC_NOT_READY,        // Notify: SD-карта не готова
    __NUMBER_OF_ASYNC_MESSAGES          // Кол-во элементов в enum
};

#endif // DBUS_CONSTANTS_H_
