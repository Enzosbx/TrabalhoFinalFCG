function M(Im)
[sx,sy,sd] = size(Im);

J2 = zeros(2*sx, 2*sy, sd);
for d = 1:sd
    for i = 1:sx
        for j = 1:sy
            J2(2*i, 2*j, d) = Im(i,j,d);
        end
    end
    for i = 2:sx
        for j = 2:sy
            J2(2*i-1, 2*j, d) = (double(J2(2*i,2*j,d))+ double(J2(2*i-2,2*j,d)))/2;
            J2(2*i, 2*j-1, d) = (double(J2(2*i,2*j,d))+ double(J2(2*i,2*j-2,d)))/2;
        end
    end
    for i = 2:sx
        for j = 2:sy
            J2(2*i-1, 2*j-1, d) = (double(J2(2*i,2*j,d))+ double(J2(2*i-2,2*j,d))+double(J2(2*i,2*j-2,d))+ double(J2(2*i-2,2*j-2,d)))/4;
        end
    end
end

J2 = uint8(J2);
[sx,sy,sd] = size(J2);
sizeS = 2;
area = ((sizeS*2+1)^2);
J = zeros(sx,sy,sd);
J = J2;

for d=1:sd
    for x=sizeS+1:sx-sizeS
        for y=sizeS+1:sy-sizeS
            media = 0;
            desvio = 0;
            for i=-sizeS:sizeS
                for j=-sizeS:sizeS
                    media = media + double(J2(x-i,y-j,d));
                end
            end
            media = media/area;
            for i=-sizeS:sizeS
                for j=-sizeS:sizeS
                    desvio = desvio + (media-double(J2(x+i,y+j,d)))^2/area;
                end
            end
            desvio = sqrt(desvio);            
            if J2(x,y,d)>media
                J(x,y,d) = uint8(J2(x,y,d) + desvio/2);
            else
                J(x,y,d) = uint8(J2(x,y,d) - desvio/2);
            end
        end
    end
end

J3 = zeros(sx,sy,sd);
for d = 1:sd
    for i = 1:sx
        for j = 1:sy
        J3(i,j,d) = uint8(127+double(J(i,j,d))-double(J2(i,j,d)));
        end
    end
end

imshow(Im);
title('first');
figure;

imshow(uint8(J2));
title('just bigget');
figure

imshow(J);
title('new');
figure;

imshow(uint8(J3));
title('difference');

imwrite(J,'textura-golem3.jpg','jpg');
end

                
            
            
            
            
            
            