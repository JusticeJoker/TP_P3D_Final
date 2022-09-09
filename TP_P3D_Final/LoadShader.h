#include <GL\gl.h>
#include <GL\glew.h>

//debug
//#define _DEBUG

typedef struct {
	GLenum       type;
	const char* filename;
	GLuint       shader;
} ShaderInfo;

GLuint LoadShaders(ShaderInfo*);

/* -----------------------------------------------------------------------------------
					 GLuint LoadShaders(ShaderInfo*);

Descri��o:
----------
Recebe um array de estruturas ShaderInfo.
Cada estrutura cont�m:
- tipo de shader. No OpenGL poder� ser um dos seguintes valores:
  - GL_COMPUTE_SHADER
  - GL_VERTEX_SHADER
  - GL_TESS_CONTROL_SHADER
  - GL_TESS_EVALUATION_SHADER
  - GL_GEOMETRY_SHADER
  - GL_FRAGMENT_SHADER
- apontador para uma C-string, que cont�m o nome do ficheiro com c�digo do shader
- valor que referencia o objeto shader criado

--------
Em caso de sucesso, a fun��o retorna o valor que referencia o objeto programa.
Em caso de erro, ser� retornado o valor zero (0).

*///-----------------------------------------------------------------------------------
