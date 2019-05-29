#ifdef DEBUG
#define debug_printf printf
#else
#define debug_printf(...) do{  } while ( 0 )
#endif
