clear all;
close all;
clc;

haar2 = rgb2gray(imread('building.jpg')); 
haar2=double(haar2); 
haar2=haar2/max(max(haar2)); 
%b=b/1;
%max(max(b))
imshow(haar2);
title('Imagen Original') ; 
 
[h1,h2] = wfilters('haar','d'); 

bh1=filter2(h1,haar2); %Filtro permite suabizar la Image 
bh2=filter2(h2,haar2); %filtro permite resaltar los bordes 
figure; 

[F C]=size(bh1);%dimension de la imagen 
bh1m=bh1(:,1:2:C);% aplicamos Down sampler por colunna a la imagen del filtro pasa bajo 
bh2m=bh2(:,1:2:C);% aplicamos Down sampler por colunna a la imagen del filtro pasa Alto 


bh1mh1=filter2(h1',bh1m); % Imagen FPB se le aplica FPB, gemerando una nueva imagen FPB 
bh1mh2=filter2(h2',bh1m); % Imagen FPB se le aplica FPA, gemerando una nueva imagen FPA 
bh2mh1=filter2(h1',bh2m); % Imagen FPA se le aplica FPB, gemerando una nueva imagen FPB 
bh2mh2=filter2(h2',bh2m); % Imagen FPA se le aplica FPA, gemerando una nueva imagen FPA 

bLL=bh1mh1(1:2:F,:);% aplicamos Down sampler por fila a la imagen del filtro PB 
bLH=bh1mh2(1:2:F,:);% aplicamos Down sampler por fila a la imagen del filtro PA 
bHL=bh2mh1(1:2:F,:);% aplicamos Down sampler por fila a la imagen del filtro pasa bajo 
bHH=bh2mh2(1:2:F,:);% aplicamos Down sampler por fila a la imagen del filtro pasa bajo 

subplot(2,2,1); 

imshow(bLL/max(max(bLL))); 
title(' LL '); 
subplot(2,2,2);imshow(bLH+0.5); title(' LH '); 
subplot(2,2,3);imshow(bHL+0.5); title(' HL'); 
subplot(2,2,4);imshow(bHH+0.5); title(' HH '); 

haar2 = bLL/max(max(bLL));
figure
imshow(haar2);


bh1=filter2(h1,haar2); %Filtro permite suabizar la Image 
bh2=filter2(h2,haar2); %filtro permite resaltar los bordes 
figure; 

title('Filtro Pasa Bajo');%fitro pasa baja 

title('Filtro Pasa Alto');%fitro pasa baja 

[F C]=size(bh1);%dimension de la imagen 
bh1m=bh1(:,1:2:C);% aplicamos Down sampler por colunna a la imagen del filtro pasa bajo 
bh2m=bh2(:,1:2:C);% aplicamos Down sampler por colunna a la imagen del filtro pasa Alto 

bh1mh1=filter2(h1',bh1m); % Imagen FPB se le aplica FPB, gemerando una nueva imagen FPB 
bh1mh2=filter2(h2',bh1m); % Imagen FPB se le aplica FPA, gemerando una nueva imagen FPA 
bh2mh1=filter2(h1',bh2m); % Imagen FPA se le aplica FPB, gemerando una nueva imagen FPB 
bh2mh2=filter2(h2',bh2m); % Imagen FPA se le aplica FPA, gemerando una nueva imagen FPA 

bLL=bh1mh1(1:2:F,:);% aplicamos Down sampler por fila a la imagen del filtro PB 
bLH=bh1mh2(1:2:F,:);% aplicamos Down sampler por fila a la imagen del filtro PA 
bHL=bh2mh1(1:2:F,:);% aplicamos Down sampler por fila a la imagen del filtro pasa bajo 
bHH=bh2mh2(1:2:F,:);% aplicamos Down sampler por fila a la imagen del filtro pasa bajo 

subplot(2,2,1); 

imshow(bLL/max(max(bLL))); 
title(' LL '); 
subplot(2,2,2);imshow(bLH+0.5); title(' LH '); 
subplot(2,2,3);imshow(bHL+0.5); title(' HL'); 
subplot(2,2,4);imshow(bHH+0.5); title(' HH '); 

haar2 = bLL/max(max(bLL));
figure
imshow(haar2);

bh1=filter2(h1,haar2); %Filtro permite suabizar la Image 
bh2=filter2(h2,haar2); %filtro permite resaltar los bordes 
figure; 

[F C]=size(bh1);%dimension de la imagen 
bh1m=bh1(:,1:2:C);% aplicamos Down sampler por colunna a la imagen del filtro pasa bajo 
bh2m=bh2(:,1:2:C);% aplicamos Down sampler por colunna a la imagen del filtro pasa Alto 

bh1mh1=filter2(h1',bh1m); % Imagen FPB se le aplica FPB, gemerando una nueva imagen FPB 
bh1mh2=filter2(h2',bh1m); % Imagen FPB se le aplica FPA, gemerando una nueva imagen FPA 
bh2mh1=filter2(h1',bh2m); % Imagen FPA se le aplica FPB, gemerando una nueva imagen FPB 
bh2mh2=filter2(h2',bh2m); % Imagen FPA se le aplica FPA, gemerando una nueva imagen FPA 

bLL=bh1mh1(1:2:F,:);% aplicamos Down sampler por fila a la imagen del filtro PB 
bLH=bh1mh2(1:2:F,:);% aplicamos Down sampler por fila a la imagen del filtro PA 
bHL=bh2mh1(1:2:F,:);% aplicamos Down sampler por fila a la imagen del filtro pasa bajo 
bHH=bh2mh2(1:2:F,:);% aplicamos Down sampler por fila a la imagen del filtro pasa bajo 

subplot(2,2,1); 

imshow(bLL/max(max(bLL))); 
title(' LL '); 
subplot(2,2,2);imshow(bLH+0.5); title(' LH '); 
subplot(2,2,3);imshow(bHL+0.5); title(' HL'); 
subplot(2,2,4);imshow(bHH+0.5); title(' HH '); 

haar2 = bLL/max(max(bLL));
figure
imshow(haar2);