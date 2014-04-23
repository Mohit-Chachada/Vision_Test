% Generate datasets
function data = generateDataset(set_no)
switch set_no
    case 1
        N = 2000; % No of training samples
        d = 2; % feature vector dimensionality
        fv = zeros(N,d);
        fv(:,:) = rand(N,d);
        %figure(1);scatter(fv(:,1),fv(:,2)); axis([0 1 0 1]);
        
        labels = zeros(N,1);
        for i=1:N
            if (fv(i,1)>=0.3 && fv(i,1)<=0.7 && fv(i,2)>=0.3 && fv(i,2)<=0.7)
                labels(i) = 1;
            else
                labels(i) = -1;
            end
        end
    case 2
        N = 2000; % No of training samples
        d = 2; % feature vector dimensionality
        fv = zeros(N,d);
        fv(:,:) = rand(N,d);
        %figure(1);scatter(fv(:,1),fv(:,2)); axis([0 1 0 1]);
        
        labels = zeros(N,1);
        for i=1:N
            if ((fv(i,1)>=0.3 && fv(i,1)<=0.7 && fv(i,2)>=0.3 && fv(i,2)<=0.7) || (fv(i,1)>0.15 && fv(i,1)<0.25) || (fv(i,1)>0.75 && fv(i,1)<0.85) || (fv(i,2)>0.15 && fv(i,2)<0.25) || (fv(i,2)>0.75 && fv(i,2)<0.85))
                labels(i) = 1;
            else
                labels(i) = -1;
            end
        end        
    case 3
        N = 2000; % No of training samples
        d = 2; % feature vector dimensionality
        fv = zeros(N,d);
        fv(:,:) = 2.*randn(N,d);
        %figure(1);scatter(fv(:,1),fv(:,2)); axis([0 1 0 1]);
        
        labels = zeros(N,1);
        for i=1:N
            dist = sqrt(power(fv(i,1),2)+power(fv(i,2),2));
            if (dist<2)
                labels(i) = 1;
            else
                labels(i) = -1;
            end
        end           
    case 4
        N = 2000; % No of training samples
        d = 2; % feature vector dimensionality
        fv = zeros(N,d);
        fv(:,:) = 2.*randn(N,d);
        %figure(1);scatter(fv(:,1),fv(:,2)); axis([0 1 0 1]);
        
        labels = zeros(N,1);
        for i=1:N
            dist = sqrt(power(fv(i,1),2)+power(fv(i,2),2));
            if (dist<2 || (dist>2.5 && dist<3))
                labels(i) = 1;
            else
                labels(i) = -1;
            end
        end         
    case 5
        N = 2000; % No of training samples
        d = 169; % feature vector dimensionality        
        X_train = dlmread ('images_training.txt');
        X_train =reshape(X_train,169,1000);
        X_train = X_train';
        X_train_labels = dlmread ('labels_training.txt');
        %imshow(reshape(X_train(1,:),13,13));
        
        X_test = dlmread ('images_testing.txt');
        X_test =reshape(X_test,169,1000);
        X_test = X_test';
        X_test_labels = dlmread ('labels_testing.txt');

        train_data = X_train;
        test_data = X_test;
        half = size(X_train_labels);
        train_labels = zeros(half);
        test_labels = zeros(half);
        for i=1:half
            if (X_train_labels(i) == 2)
                train_labels(i) = 1;
            else
                train_labels(i) = -1;
            end
            if (X_test_labels(i) == 2)
                test_labels(i) = 1;
            else
                test_labels(i) = -1;
            end
        end
end


if(set_no ~= 5)
    % no of positive and negative samples
    nr_pos = 0;
    for i=1:N
        if(labels(i)==1)
            nr_pos = nr_pos + 1;
        end
    end
    nr_neg = N-nr_pos;
    
    % training set
    half = round(N/2);
    train_data = fv(1:half,:);
    train_labels = labels(1:half);
    %figure(2);scatter(train_data(:,1),train_data(:,2)); axis([0 1 0 1]);
    
    % test set
    test_data = fv(half+1:N,:);
    test_labels = labels(half+1:N);
    %figure(3);scatter(test_data(:,1),test_data(:,2)); axis([0 1 0 1]);
end

% construct output struct
data = struct('train_data',train_data,'train_labels',train_labels,'test_data',test_data,'test_labels',test_labels);
end