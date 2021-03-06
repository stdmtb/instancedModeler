#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : require
#extension GL_ARB_draw_instanced : enable

uniform sampler2D vtxtex;
uniform sampler2D cltex;

//varying vec3 normal, screenSpaceNormal;
//flat varying vec3  screenSpaceflatNormal;

varying vec3 lightDir, eyeVec;
flat varying vec3  flatNormal;

varying vec2 texCoord;
varying vec4 customColor;

int tex_w = 2048;
int tex_h = 2048;
int cltex_w = 1024;
int cltex_h = 1024;


float tex_w_f = float(tex_w);
float tex_h_f = float(tex_h);
float cltex_w_f = float(cltex_w);
float cltex_h_f = float(cltex_h);



void main()
{
    int y = (gl_InstanceID * 4) / tex_w;
    float y_f = float(y);
    float v = y_f / tex_h_f;

    mat4 mvp = mat4(texture2D(vtxtex,vec2(float((gl_InstanceID*4+0)%tex_w)/tex_w_f, v)),
                    texture2D(vtxtex,vec2(float((gl_InstanceID*4+1)%tex_w)/tex_w_f, v)),
                    texture2D(vtxtex,vec2(float((gl_InstanceID*4+2)%tex_w)/tex_w_f, v)),
                    texture2D(vtxtex,vec2(float((gl_InstanceID*4+3)%tex_w)/tex_w_f, v)) );
    
    mat3 rotMat = mat3(mvp[0].xyz, mvp[1].xyz, mvp[2].xyz);

    // shading
    vec3 inNormal = rotMat * gl_Normal;
    vec4 inVertex = mvp * gl_Vertex;
    
//    screenSpaceNormal = (gl_ModelViewMatrix * vec4(inNormal,1.0)).xyz;
//    screenSpaceflatNormal = (gl_ModelViewMatrix * vec4(inNormal,1.0)).xyz;
//    normal = gl_NormalMatrix * inNormal;
    flatNormal = gl_NormalMatrix * inNormal;
    vec3 vVertex = vec3(gl_ModelViewMatrix * inVertex);
    lightDir = vec3(gl_LightSource[0].position.xyz - vVertex);
    eyeVec = -vVertex;
    texCoord = gl_MultiTexCoord0.xy;
    
    // position
  	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * inVertex;
    

    {
        // color
        int y = gl_InstanceID/cltex_w;
        float y_f = float(y);
        float v = y_f / cltex_h_f;
        
        customColor = texture2D(cltex, vec2(float(gl_InstanceID%cltex_w)/cltex_w_f, v));
    }
}
