#version 330                                                                        

uniform uint uSelected;  
uniform uint isbutton;                                                                          

layout(location = 0) out vec4 FragColor;                                            
                                                                                    
void main()                                                                         
{
	if(uSelected != uint(0)) 
	{
		FragColor = vec4(1.0, isbutton, 0.0, 1.0);
	}
	else
	{
		FragColor = vec4(1.0, isbutton, 1.0, 1.0);
	}                                                                                                                           
}