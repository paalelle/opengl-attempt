#version 420 core

const float PI = 3.14159265;
const float PI2 = 6.2831853;
const float PIf2 = 1.570796327;
const float INF = 2147483647;
const float PREC = 0.000001;

struct Sphere{
    vec3 position;
    float radius;
    vec3 color;
    float roughness;
    float metallic;
    float emission;
};

struct Plane{
    vec3 point;
    vec3 normal;
    vec3 color;
    float roughness;
    float metallic;
    float emission;
};

struct RayHit{
    bool isHit;
    vec3 position;
    vec3 normal;
    float dist;     //���е㵽��ʼ�����

    int hitType;    //������������    0-����, 1-ƽ��
    int hitIndex;   //���������±�
};

uniform float randSeed;
uniform float randSeed2;

uniform vec3 cameraPos;
uniform float cameraFOV;
uniform mat4 cameraRotationMat;
uniform vec2 screenScale;

uniform vec3 backgroundColor;

//�洢��������� uniform ��
layout (std140, binding = 0) uniform ObjectBlock{
    int sphereCount;
    int planeCount;
    Sphere spheres[64];
    Plane planes[64];
};

in vec2 texCoords;
out vec4 fragColor;

//===========================================================================

float randSeedN = fract(randSeed * randSeed2);
float RandXY(float x, float y){
     return fract(sin(x * 12.9898 + y * 78.233) * 43758.5453);
}
//α�������,����[0,1)�ϵ������
float Rand(){
    float randtmp = RandXY(texCoords.x, texCoords.y);
    float randtmp2 = RandXY(randSeed, randSeedN+=1);
    float randtmp3 = RandXY(randtmp, randSeed2);
    return RandXY(randtmp2, randtmp3);
}



//��LookAt�任Ӧ��������tvec
void LookAt(vec3 lookvec, vec3 upvec, inout vec3 tvec){
    vec3 rvec = cross(lookvec, upvec);
    mat3 LM = mat3(rvec, upvec, lookvec);
    tvec = LM * tvec;
}

//��RayHit�ṹ�����л�ȡ�����������
void GetPropFromHit(RayHit hitInfo, out vec3 co, out float ro, out float me){
    switch(hitInfo.hitType){
        case 0:
            co = spheres[hitInfo.hitIndex].color;
            ro = spheres[hitInfo.hitIndex].roughness;
            me = spheres[hitInfo.hitIndex].metallic;
            break;
        case 1:
            co = planes[hitInfo.hitIndex].color;
            ro = planes[hitInfo.hitIndex].roughness;
            me = planes[hitInfo.hitIndex].metallic;
            break;

        default:
            co = vec3(0.0);
            ro = me = 0.0;
            break;
    }
}
void GetEmitFromHit(RayHit hitInfo, out vec3 co, out float em){
    switch(hitInfo.hitType){
        case 0:
            co = spheres[hitInfo.hitIndex].color;
            em = spheres[hitInfo.hitIndex].emission;
            break;
        case 1:
            co = planes[hitInfo.hitIndex].color;
            em = planes[hitInfo.hitIndex].emission;
            break;

        default:
            co = vec3(0.0);
            em = 0.0;
            break;
    }
}


//���߷ֲ�������
float D_GGX(vec3 N, vec3 halfvec, float alpha){
    float nom = alpha * alpha;
    float denom = dot(N, halfvec);
    denom *= denom * (nom-1) + 1;
    denom *= denom * PI;
    return nom / denom;
}
//���κ�����
float G_SchlickGGX(vec3 N, vec3 v, float k){
    float nvdot = max(0, dot(N, v));
    return nvdot / (nvdot * (1-k) + k);
}
float G_Smith(vec3 N, vec3 vin, vec3 vout, float k){
    float nvdot = max(0, dot(N, vout));
    float ret = nvdot / (nvdot * (1-k) + k);
    nvdot = max(0, dot(N, vin));
    ret *= nvdot / (nvdot * (1-k) + k);
    return ret;
    //return G_SchlickGGX(normal, vout, k) * G_SchlickGGX(normal, vin, k);
}
//Fresnel��
vec3 Fresnel(vec3 halfvec, vec3 view, vec3 F0){
    return F0 + (1-F0) * pow(1 - max(0, dot(halfvec, view)), 5.0);
}
vec3 BRDF(vec3 rayin, vec3 rayout, RayHit hitInfo){
    vec3 co;
    float ro, me, em;
    vec3 N = hitInfo.normal;

    GetPropFromHit(hitInfo, co, ro, me);
    //һ��������Ԥ��ֵ
    vec3 F0 = vec3(0.04);
    //�������
    vec3 hvec = normalize(rayin + rayout);
    F0 = mix(F0, co, me);

    //���淴��ϵ����������ϵ��
    vec3 k_s = Fresnel(hvec, rayout, F0);
    //vec3 k_d = (1.0 - k_s) * (1.0 - me);
    //���ǵ�fresnel��ͨ����С,���㹻��ʱ�������Ծ��淴��Ϊ����,ֱ�Ӻ���֮
    float k_d = 1.0 - me;

    //���淴�����
    vec3 spec = D_GGX(N, hvec, ro) * G_Smith(N, rayin, rayout, (ro+1)*(ro+1)/8) * k_s;
    spec /= 4 * max(0, dot(rayin, N)) * max(0, dot(rayout, N)) + 0.001;

    return k_d * co / PI + spec;
}

//ray��Ҫ�Ǳ�׼������
void GetIntersectionSphere(vec3 oripos, vec3 ray, int targetID, out RayHit hitInfo){
    vec3 pos = spheres[targetID].position;
    float rad = spheres[targetID].radius;
    //��ʼ���������ڲ�
    if(distance(oripos, pos) < rad){
        hitInfo.isHit = false;
        return;
    }

    float t = dot(ray, pos - oripos) / dot(ray, ray);

    //�����������߷�����
    if(t < 0){
        hitInfo.isHit = false;
        return;
    }

    //�������ߵ����ĵ���̾���
    float dis = distance(oripos + t*ray, pos);
    if(dis > rad){
        hitInfo.isHit = false;
        return;
    }
    t = t - sqrt(rad*rad - dis*dis);
    hitInfo.position = oripos + t*ray;
    hitInfo.dist = t;
    hitInfo.normal = normalize(hitInfo.position - pos);
    hitInfo.isHit = true;
    
}
void GetIntersectionPlane(vec3 oripos, vec3 ray, int targetID, out RayHit hitInfo){
    vec3 pos = planes[targetID].point;
    vec3 nor = planes[targetID].normal;

    if(abs(dot(nor,ray)) < PREC){
        hitInfo.isHit = false;
        return;
    }
    float t = dot(pos - oripos, nor) / dot(nor, ray);

    //��ƽ�������߷�����
    if(t < 0){
        hitInfo.isHit = false;
        return;
    }

    hitInfo.position = oripos + t*ray;
    hitInfo.dist = t;
    hitInfo.normal = nor;
    hitInfo.isHit = true;
}



void HitQuery(vec3 rayori, vec3 ray, out RayHit targetHitInfo){
    RayHit tmphitInfo;
    targetHitInfo.isHit = false;
    targetHitInfo.dist = INF;
    for(int i=0; i<sphereCount; i++){
        GetIntersectionSphere(rayori, ray, i, tmphitInfo);
        if(tmphitInfo.isHit && tmphitInfo.dist < targetHitInfo.dist){
            targetHitInfo = tmphitInfo;
            targetHitInfo.hitType = 0;
            targetHitInfo.hitIndex = i;
        }
    }
    for(int i=0; i<planeCount; i++){
        GetIntersectionPlane(rayori, ray, i, tmphitInfo);
        if(tmphitInfo.isHit && tmphitInfo.dist < targetHitInfo.dist){
            targetHitInfo = tmphitInfo;
            targetHitInfo.hitType = 1;
            targetHitInfo.hitIndex = i;
        }
    }
}
float prob_RR = 0.6;
const int maxDepth = 32;
vec3 recurStack[maxDepth+1];
int stackTop = 0;
vec3 RayTrace(vec3 ray){
    vec3 R = backgroundColor;
    //ÿ�ε��������ߵ���ʼ��
    vec3 rayori;
    ray = normalize(ray);
    RayHit hitInfo;

    HitQuery(cameraPos, ray, hitInfo);

    stackTop = 0;
    while(stackTop < maxDepth){
        //δ�����κ�����,ֹͣ�ݹ�
        if(!hitInfo.isHit){
            return R;
        }

        vec3 co;
        float em;
        GetEmitFromHit(hitInfo, co, em);
        //return hitInfo.normal;
        //�����бȽ����ķ�����,ֹͣ�ݹ�
        if(em > 0.5){
            recurStack[stackTop] = co * em;
            break;
        }


        //Russian Roulette Test
        float tst_RR = Rand();
        if(tst_RR > prob_RR) return vec3(0.0);

        //�����������
        //����ڰ������ϲ���
        float t_theta = Rand() * PI2;
        float t_phi = Rand() * PIf2;

        //����ݹ��������
        vec3 nextray = vec3(sin(t_phi)*cos(t_theta), sin(t_phi)*sin(t_theta), cos(t_phi));
        //���ȡһ����ֱ�ڷ������ĵ�λ������Ϊ��������
        vec3 normalRight;
        if(hitInfo.normal.x != 0 || hitInfo.normal.y != 0) normalRight = normalize(vec3(-hitInfo.normal.y, hitInfo.normal.x, 0));
        else normalRight = normalRight = vec3(1.0, 0, 0);

        LookAt(hitInfo.normal, normalRight, nextray);
        nextray = normalize(nextray);
        //return hitInfo.normal;
        //return nextray;
        //nextray = reflect(ray, hitInfo.normal);
        //if(stackTop > 0)
            //return hitInfo.normal;
            //return nextray;

        vec3 R_directFact = vec3(0.0);
        vec3 R_indirectFact = vec3(0.0);
        R_indirectFact = BRDF(-ray, nextray, hitInfo) * dot(hitInfo.normal, nextray) * PI2 / prob_RR;
        //R_indirectFact = vec3(0.5);
        recurStack[stackTop++] = R_directFact + R_indirectFact + co * em;

        //����һ��ƫ����,���������ߴ���ԭ����������
        rayori = hitInfo.position + nextray * 0.0001;
        ray = nextray;
        HitQuery(rayori, ray, hitInfo);
        
    }

    //����
    R = recurStack[stackTop];
    for(int i=stackTop-1; i>=0; i--){
        R *= recurStack[i];
    }
    return R;
}

void main(){
       
    //fragColor = vec4(vec3(spheres[1].emission), 1.0);
    //return;
    
    //x,y�����������-1��1֮��
    //vec2 fixedCoord = 2.0 *((gl_FragCoord.xy - vec2(0.5, 0.5)) / screenScale - 0.5);
    vec2 fixedCoord = 2.0 * (texCoords - 0.5);
    //ע���ʼ����ŷ����Ϊ0�ķ�����z�Ḻ����
    //�����������: viewVec = (x,y,z), ֻ��ǰ��������������, z��ȷ��Ϊ1���ٱ�׼��
    float viewLimX = tan(cameraFOV / 2);
    float viewLimY = viewLimX * screenScale.y / screenScale.x;
    vec2 pixelSize = 5.0*vec2(viewLimX / screenScale.x, viewLimY / screenScale.y);
    vec4 viewVec = vec4(fixedCoord.x * viewLimX, fixedCoord.y * viewLimY, -1.0, 0.0);
    
    vec3 tracedColor = vec3(0.0);
    int totSample = 10;
    //totSample = 1;
    for(int n=0; n<totSample; n++){
        vec2 randPix = vec2(Rand(), Rand());
        vec4 randViewVec = normalize(vec4(randPix * pixelSize, 0.0, 0.0) + viewVec);
        randViewVec = normalize(randViewVec);
        randViewVec = cameraRotationMat * randViewVec;
        tracedColor = (tracedColor * n + RayTrace(randViewVec.xyz)) / (n+1);
    }

    tracedColor = tracedColor / (tracedColor + vec3(1.0));
    tracedColor = pow(tracedColor, vec3(1.0/2.2)); 

    //fragColor = vec4(1.0, 1.0, 1.0, 1.0);
    fragColor = vec4(tracedColor, 1.0);
    //fragColor = vec4(-vec3(planes[3].normal), 1.0);
    //fragColor = vec4((fixedCoord + 1.0) / 2, 1.0, 1.0);
    //fragColor = vec4(viewVec.xyz, 1.0);
}
