%module _remote_pause
   %include typemaps.i
   %{
	void set_handlers(void);
	int get_signal_state(void);
    %}

void set_handlers(void);
int get_signal_state(void);
