namespace io {
typedef enum input_event { NIL, SELECT, FORWARD, BACKWARD } input_event;

inline input_event read_encoder();

inline void play_buzzer(float frequency, long duration);

inline void        distance_sensor_init();
inline const float read_distance_sensor();

inline void        weight_sensor_init();
inline void        calibrate_weight_sensor();
inline const float read_weight_sensor();
} // namespace io
