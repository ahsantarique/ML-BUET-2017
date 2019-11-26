K = 20;
D = 200;
toprint = 5;    % # of words to print in output

W = [];    %corpus


for i=1:D
    file = num2str(i);
    temp = textread(file, '%s', 'delimiter', ' ');
    doc{i} = temp;
    doccount{i} = size(temp,1);  %wordcount in doc i
    W = [W ; temp(:)];
end;

N = size(W);  %N = total #of words in corpus 

vocab = unique(W);

V= size(vocab, 1);

Z = floor(rand(size(W))*K)+1;  %randomly assign topic to each word in corpus

ndel = zeros(D,K);

cum = 0;
d = 0;
for i = 1:N
    if(i > cum)
        d = d + 1;
        cum = cum + doccount{d};
    end
    t = Z(i,1);
    ndel(d,t) = ndel(d,t) + 1;
end;

n = zeros(K,V);

for i = 1:N
    t = Z(i,1);
    w = find(strcmp(vocab,W(i,1)));  % w is an index in vocab
    n(t,w) = n(t,w) + 1;
end;
    
P = zeros(1,K);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%  fix parameters %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
alpha = 50/K;
eta = 0.1;
maxiter = 500;
burniter = 0;
lag = 1;

cumn = zeros(size(n));
for iter = 1:maxiter
    cum = 0;
    d = 0;
    for i = 1:N
        if(i > cum)
            d = d + 1;
            cum = cum + doccount{d};
        end
        % i-th word is in doc d
        
        word = W(i,1);
        w = find(strcmp(word,vocab)); % w is an index in vocab


        %%%%%%%%%%%%%%%%%  calc topic probability %%%%%%%%%%%%%%%%%%%%%%%%%
        sump = 0;
        for t = 1:K
            P(1,t) = (alpha+ndel(d,t)) * (eta + n(t,w)) / ((K*alpha +sum(ndel(d,:))) * (V*eta+sum(n(t,:))));
            sump = sump + P(1,t);
        end
        %%%%%%%%%%%%%%%% normalize %%%%%%%%%%%%%%%%%%%%
        c = 0;
        cumsum = zeros(K);
        for t = 1:K
            P(1,t) = P(1,t) / sump;
            cumsum(t) = c + P(1,t); 
            c = cumsum(t);
        end
        
        r = rand(1,1);
        %%%%%%%%%%%%%% sample %%%%%%%%%%%%%%%%%%%%%%%%%%
        selected_topic=0;
        for t=1:K
            if(r <= cumsum(t))
                selected_topic = t;
                break;
            end
        end
        %%%%%%%%%%%%%%%%%%%%%%%%%%
        %disp(selected_topic);
        oldtopic = Z(i,1);
        ndel(d,oldtopic) = ndel(d,oldtopic) - 1;
        n(oldtopic,w) = n(oldtopic,w) -1;
        
        Z(i,1) = selected_topic;
        ndel(d,selected_topic) = ndel(d,selected_topic) + 1;
        n(selected_topic,w) = n(selected_topic,w) + 1;
    end
    if(iter > burniter && mod(iter,lag)==0)
        cumn = cumn + n;
    end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%% results %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

for t = 1:K
    [B, indices] = sort(cumn(t,:), 'descend');
    disp(vocab(indices(1:toprint)));
end

            
        