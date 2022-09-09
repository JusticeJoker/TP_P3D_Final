#version 440 core

uniform mat4 Model;
uniform mat4 View;
uniform mat4 ModelView;		// View * Model
uniform mat4 Projection;
uniform mat3 NormalMatrix;
uniform float time;

//effect in model
uniform float effectModel;

in vec3 vPosition;			// Coordenadas locais do v�rtice
in vec3 vNormal;			// Normal do v�rtice
in vec2 vTextureCoords;  //recebe coordenadas UV

out vec3 vPositionEyeSpace;
out vec3 vNormalEyeSpace;
out vec3 textureVector;
out vec2 textureCoord;   //envia coordenadas UV

void main()
{ 
	// Posi��o do v�rtice em coordenadas do olho.
	vPositionEyeSpace = (ModelView * vec4(vPosition, 1.0)).xyz;

	// Transformar a normal do v�rtice.
	vNormalEyeSpace = normalize(NormalMatrix * vNormal);

	// Coordenada de textura para o CubeMap
	textureVector = vPosition;

	//UV
	textureCoord = vTextureCoords;
	
	if(effectModel != 0)
	{
		// Posi��o final do v�rtice (em coordenadas de clip)
		// Posi��o final do v�rtice (em coordenadas de clip)
		//gl_position contem a variavel da posis�o do vertice 
		//abs � para retornar sempre um valor positivo e nunca negativo 
		//sin() � para retornar sempr eum valor compreendido entre 0 e 1
		//desta forma n�o precisamos de preocupar com as mormais porque o valor � sempre escalar 
		//caso fosse pro um formato mais tradicional teriamos de usar a inversa da transposta para as normais se manter corretas
		//gl_Position = Projection * ModelView * vec4(vPosition*abs(sin(time + vPosition.y + vPosition.x))/* Deforma�ao em funcao do tempo*/, 1.0f);
	
		//gl_Position = Projection * ModelView * vec4(vPosition.x * abs(sin(time)), vPosition.y * abs(sin(time + vPosition.y)), vPosition.z * abs(sin(time)), 1.0f);

		//gl_Position = Projection * ModelView * vec4(vPosition.x, vPosition.y * clamp(abs(sin(time + vPosition.y)), 0.4f, 0.6f), vPosition.z , 1.0f);

		//gl_Position = Projection * ModelView * vec4(vPosition.x, vPosition.y , vPosition.z * clamp(abs(sin(time + vPosition.z)), 0.5f, 0.8f), 1.0f);
	
		//Melhor op��o para observar luzes e transmo��es
		//gl_Position = Projection * ModelView * vec4(vPosition.x  * clamp(abs(sin(time + vPosition.y)), 0.3f, 0.7f), vPosition.y , vPosition.z, 1.0f);
	
		//n�o fica est�tico no lugar por isso as luzes deixam de afetar 
		//melhor transforma��o
		gl_Position = Projection * ModelView * vec4(vPosition.x  + clamp(sin(time + vPosition.y), -0.1f, 0.1f), vPosition.y , vPosition.z, 1.0f);

		//gl_Position = Projection * ModelView * vec4(vPosition*abs(sin(time))/* Deforma�ao em funcao do tempo*/, 1.0f);	
		//gl_Position = Projection * ModelView * vec4(vPosition + abs(time * sin(vPosition.y)), 1.0f);

		//para fazer uma transforma��o na vertical e na horizontal provavelmente teria de ser aqui tb 
		//era usar a mesma estrat�gia s� que com uma matriz alterada 
		//depois nao sei se temos aqui a funcao da inversa da transposta 
		//ou teriamos de a calcular manualmente 
		//seria interessante fazer isso 
		//PS: O prof n quer isto 
	}
	else
	{
		//Normal representation
		gl_Position = Projection * ModelView * vec4(vPosition, 1.0f);
	}


}
