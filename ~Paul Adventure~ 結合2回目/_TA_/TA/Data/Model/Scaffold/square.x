xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 30;
 0.00000;0.10050;-1.67900;,
 1.31269;0.10050;-1.04684;,
 1.31269;-0.10050;-1.04684;,
 0.00000;-0.10050;-1.67900;,
 1.63690;0.10050;0.37361;,
 1.63690;-0.10050;0.37361;,
 0.72849;0.10050;1.51273;,
 0.72849;-0.10050;1.51273;,
 -0.72849;0.10050;1.51273;,
 -0.72849;-0.10050;1.51273;,
 -1.63690;0.10050;0.37361;,
 -1.63690;-0.10050;0.37361;,
 -1.31269;0.10050;-1.04684;,
 -1.31269;-0.10050;-1.04684;,
 0.00000;0.10050;-1.67900;,
 0.00000;-0.10050;-1.67900;,
 0.00000;0.10050;0.00000;,
 0.00000;0.10050;0.00000;,
 0.00000;0.10050;0.00000;,
 0.00000;0.10050;0.00000;,
 0.00000;0.10050;0.00000;,
 0.00000;0.10050;0.00000;,
 0.00000;0.10050;0.00000;,
 0.00000;-0.10050;-0.00000;,
 0.00000;-0.10050;-0.00000;,
 0.00000;-0.10050;-0.00000;,
 0.00000;-0.10050;-0.00000;,
 0.00000;-0.10050;-0.00000;,
 0.00000;-0.10050;-0.00000;,
 0.00000;-0.10050;-0.00000;;
 
 21;
 4;0,1,2,3;,
 4;1,4,5,2;,
 4;4,6,7,5;,
 4;6,8,9,7;,
 4;8,10,11,9;,
 4;10,12,13,11;,
 4;12,14,15,13;,
 3;16,1,0;,
 3;17,4,1;,
 3;18,6,4;,
 3;19,8,6;,
 3;20,10,8;,
 3;21,12,10;,
 3;22,14,12;,
 3;23,3,2;,
 3;24,2,5;,
 3;25,5,7;,
 3;26,7,9;,
 3;27,9,11;,
 3;28,11,13;,
 3;29,13,15;;
 
 MeshMaterialList {
  1;
  21;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;;
  Material {
   0.800000;0.000000;0.050196;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
 }
 MeshNormals {
  9;
  0.000000;1.000000;0.000000;,
  0.000000;0.000000;-1.000000;,
  0.781832;0.000000;-0.623490;,
  0.974928;0.000000;0.222521;,
  0.433884;0.000000;0.900969;,
  -0.433884;0.000000;0.900969;,
  -0.974928;0.000000;0.222521;,
  -0.781831;0.000000;-0.623490;,
  0.000000;-1.000000;-0.000000;;
  21;
  4;1,2,2,1;,
  4;2,3,3,2;,
  4;3,4,4,3;,
  4;4,5,5,4;,
  4;5,6,6,5;,
  4;6,7,7,6;,
  4;7,1,1,7;,
  3;0,0,0;,
  3;0,0,0;,
  3;0,0,0;,
  3;0,0,0;,
  3;0,0,0;,
  3;0,0,0;,
  3;0,0,0;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;,
  3;8,8,8;;
 }
 MeshTextureCoords {
  30;
  0.000000;0.000000;,
  0.142857;0.000000;,
  0.142857;1.000000;,
  0.000000;1.000000;,
  0.285714;0.000000;,
  0.285714;1.000000;,
  0.428571;0.000000;,
  0.428571;1.000000;,
  0.571429;0.000000;,
  0.571429;1.000000;,
  0.714286;0.000000;,
  0.714286;1.000000;,
  0.857143;0.000000;,
  0.857143;1.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.071429;0.000000;,
  0.214286;0.000000;,
  0.357143;0.000000;,
  0.500000;0.000000;,
  0.642857;0.000000;,
  0.785714;0.000000;,
  0.928571;0.000000;,
  0.071429;1.000000;,
  0.214286;1.000000;,
  0.357143;1.000000;,
  0.500000;1.000000;,
  0.642857;1.000000;,
  0.785714;1.000000;,
  0.928571;1.000000;;
 }
}
