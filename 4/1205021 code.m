clear; clc;
%%%%%%%%%%%%% define parameters %%%%%%%%%%%%%%%%
M = 3;
maxiter = 500;
%%%%%%%%%%%%% load data %%%%%%%%%%%%%%%
x = load('loc.txt');

N = size(x,1);
d = size(x,2);

%%%%%%%%%%%%%% initialize variables %%%%%%%%
z = rand(N,M);
z = double(z==max(z,[],2)); %randomly assign z
theta = sum(z)/N;
mu = rand(M,d);
sigma2 = zeros(d,d,M);
index = randperm(N,M);

for m =1:M
    %mu(:,m) = x(index(m),:);
    sigma2(:,:,m) = cov(x);
    %sigma2(:,:,m) = (sigma2(:,:,m)+sigma2(:,:,m)')/2;
end

for iter = 1:maxiter
    %% E step
     for i = 1:N
         p = zeros(M,1);
         pthetatotal = 0;
         for m=1:M
             p(m,1)= multivariateGaussian(x(i,:),mu(m,:), sigma2(:,:,m));
             pthetatotal = pthetatotal + p(m,1) * theta(m);
             %disp(pthetatotal);
         end
         for m=1:M
             z(i,m) = p(m,1)*theta(m)/pthetatotal;
             %disp(z(i,m));
         end
     end
     %% M step
     theta = sum(z)/N;
     mu = z'*x;
     for m = 1:M
         mu(m,:) = mu(m,:)/sum(z(:,m));
     end
     sigma2 = zeros(d,d,M);
     for m = 1:M
         for i=1:N
             sigma2(:,:,m) = sigma2(:,:,m)+ z(i,m)*(x(i,:)-mu(m,:))'*(x(i,:)-mu(m,:));
         end
         sigma2(:,:,m) = sigma2(:,:,m)/sum(z(:,m));
     end
end

plot(x(:,1),x(:,2),'.');
p = min(x(:,1)):(max(x(:,1))-min(x(:,1)))/100:max(x(:,1)); %// x axis
q = min(x(:,2)):(max(x(:,2))-min(x(:,2)))/100:max(x(:,2)); %// y axis

hold on;
for m=1:M
    [X, Y] = meshgrid(p,q); 
    Z = mvnpdf([X(:) Y(:)],mu(m,:),sigma2(:,:,m)); %// compute Gaussian pdf
    Z = reshape(Z,size(X)); %// put into same size as X, Y
    %contour(X,Y,Z);% axis equal  %// contour plot; set same scale for x and y...
    surf(X,Y,Z);
end
hold off;

figure(2);
plot(x(:,1),x(:,2),'.');
hold on;
for m=1:M
    plot(mu(m,1), mu(m,2), 'O', 'LineWidth', 5);
end
hold off;
disp('Enemy ship positions:');
disp(mu);


function p = multivariateGaussian(X, mu, Sigma2)
k = length(mu);
if (size(Sigma2, 2) == 1) || (size(Sigma2, 1) == 1)
    Sigma2 = diag(Sigma2);
end
X = bsxfun(@minus, X, mu(:)');
p = (2 * pi) ^ (- k / 2) * det(Sigma2) ^ (-0.5) *exp(-0.5 * sum(bsxfun(@times, X * pinv(Sigma2), X), 2));

end
