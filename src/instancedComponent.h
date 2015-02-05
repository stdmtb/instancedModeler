//
//  instancedComponent.h
//


//
//  Data Hierarchy
//
//  model - component - instance - mesh
//
#pragma once

#include "myDef.h"
#include "ofMain.h"
#include "ofxVboMeshInstanced.h"
#include "ofxMtb.h"
#include "btBulletCollisionCommon.h"

#include <map>

// texture for VERTEX
#define VTX_TEX_WIDTH       2048    // 2048*2048/4 = 1,048,576
#define VTX_TEX_HEIGHT      2048

// texture for COLOR
#define CL_TEX_WIDTH        1024     // 1024 * 1024 = 1,048,576
#define CL_TEX_HEIGHT       1024


enum INSTANCE_TYPE{
    INSTANCE_NONE = 0,
    INSTANCE_SPHERE = SPHERE_SHAPE_PROXYTYPE,
    INSTANCE_CYLINDER = CYLINDER_SHAPE_PROXYTYPE
};

struct instance{
public:
    instance(const instance& i)
    :color(i.color), type(i.type), matrix(i.matrix),scale(i.scale){}
    
    ofMatrix4x4     matrix;     // NOTICE dont set scaling value
    ofVec3f         scale;
    ofFloatColor    color;
    INSTANCE_TYPE   type;
    int             groupId;
    instance(){};
    
    bool operator==(const instance& o){

        if(type == o.type)
            if(scale == o.scale)
                if(matrix.getTranslation() == o.matrix.getTranslation())
                        return true;
        return false;
    }
    
    
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version){
        ofVec3f pos         = matrix.getTranslation();
        ofQuaternion quat   = matrix.getRotate();
        ofVec3f axis;
        float angle;
        quat.getRotate(angle, axis);
        axis.normalize();

        ar  & pos.x & pos.y & pos.z
            & angle & axis.x & axis.y & axis.z
            & scale.x & scale.y & scale.z
            & type;
        
        if(version==3){
            ar & groupId;
        }
    }

    
};


typedef multimap<int, instance> INSTANCE_MAP;
typedef INSTANCE_MAP::iterator INSTANCE_MAP_ITR;

class instancedComponent{
    
private:
    static INSTANCE_MAP instanceMap;
    static int groupIdMaster;
    static int groupTotalNum;
    
    bool    bVtxtexNeedUpdate;
    bool    bCltexNeedUpdate;
    int     instanceNum;
        
    GLuint  vtxtexId;
    GLuint  cltexId;

    INSTANCE_TYPE insType;
    
    ofxVboMeshInstanced * vmi;

    
public:
    instancedComponent();
    instancedComponent(INSTANCE_TYPE t, INSTANCE_MAP& m);
    ~instancedComponent();
    
    static INSTANCE_MAP &getInstanceMap(){ return instanceMap; }
    
    void destroy();
    void reset();

    void setInstanceType(INSTANCE_TYPE t);

    void update();
    void updateVertexTexture();
    void updateColorTexture();
    void updateRequest();
    
    void draw(ofShader * shader);
    void drawWireframe(ofShader * shader);
    static void debugDraw();
    
    void loadInstanceMesh(ofMesh mesh, ofVec3f scale=ofVec3f(1,1,1));
    void loadInstancePositionFromModel(string path, int res, float posScale);
    void loadInstancePositionFromMatrices(ofMatrix4x4 * ms, ofVec3f * ss, int size);

    // instance param
    void addInstance(instance ins, int groupId=-1);
    void addInstanceMatrix      (ofMatrix4x4 m, ofVec3f s, int groupId=-1);
    void clearInstanceMatrices();
    void setInstanceColor(INSTANCE_MAP_ITR itr, ofFloatColor color);
    void setInstanceColor(int index, ofFloatColor color);
    void updateInstanceNum();
  
    
    // group func
    static void resetGroup();
    static void changeInstanceGroupId(INSTANCE_MAP_ITR& itr, int groupId);
    static void mergeInstanceGroup(int groupIdA, int groupIdB);
    static void mergeInstanceGroupAll(int groupId);
    static int  updateGroupTotalNum();
    static int  getGroupTotalNum(){ return groupTotalNum;}
    
    void removeGroup(int groupId);
    void removeSmallGroup(int minNum);
    void setGroupColor(int groupId, ofFloatColor color);
    void setGroupColorGradient();
    
    static vector<string> printData(bool outputConsole=true);
    static vector<string> printGroupData(bool outputConsole=true);

    // validate
    static void removeDuplication();
    
    // accessor
    inline static int getGroupIdMaster(){ return groupIdMaster;}
    inline static int incGroupIdMaster(){ return ++groupIdMaster; }
    inline void setInstanceNum(int i){ instanceNum = i; }
    inline int getInstanceNum(){ return instanceNum; }
    
    inline void setCltexNeedUpdate(bool b){ bCltexNeedUpdate = b; }
    inline void setVtxtexNeedUpdate(bool b){ bVtxtexNeedUpdate = b; }
    inline ofxVboMeshInstanced * getVboMeshInstanced(){ return vmi; }
  
    
    // util
    void saveInstanceDataToCsv(string dirName);
    void loadInstanceDataFromCsv(string dirName);
    
    template<class Archive>

    void serialize(Archive & ar, const unsigned int version){
        ar  & instanceMap & insType;
    }
    
};