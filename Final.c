	// Create file pointer to config file, set to read configureMcConfigureFace
	FILE* configFile;
	configFile = fopen("/home/pi/configureMcConfigureFace.cfg", "r");
   
    /*
    This section of the code gets info from the
    config file to configure the program
    */
   	int timeout = 0;
	char logFileName[50];
	int CaesarShift = 0;
	
	// readConfig function called to read from config file
	readConfig(configFile, &timeout, logFileName, &CaesarShift);
    
    //Check that the file opens properly.
	if(!configFile)
	{
		perror("The config file could not be opened");
		return -1;
	}
    
