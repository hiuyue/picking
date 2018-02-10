#version 330                                                                        
                                                                                    
layout (location = 0) in vec3 Position;                                             
 
uniform uint isbutton;
uniform mat4 mp_M;                                                                  
uniform mat4 mp_V;   
uniform mat4 mp_P;                                                                  
                                                                                    
void main()                                                                         
{                                                                                   
	if(isbutton == uint(0))
	{	
		gl_Position = mp_P * mp_V * mp_M * vec4(Position, 1.0);                                       
	}
	else
	{
		gl_Position = vec4(Position, 1.0);
	}	                                  
}