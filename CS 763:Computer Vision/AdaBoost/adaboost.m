clear;
clc;
% Training + Test Data
set_no = input('input set no {1,2,3,4,5(images)}: ');
data = generateDataset(set_no);
train_data = data.train_data;
train_labels = data.train_labels;
test_data = data.test_data;
test_labels = data.test_labels;

% No of training samples & dimensionalty of feature vector
[N,d] = size(train_data);

% no of AdaBoost rounds
T = 40;

% Initialize training sample weights
D = (1/N)*ones(N,1);
Dt = zeros(N,T);
Dt(:,1) = D;

% Initialize stage outputs
% t th stage weak classifier
index = zeros(1,T);
threshold = zeros(1,T);
parity = zeros(1,T);
alpha = zeros(1,T);
gamma = zeros(1,T);
% upto t th stage strong classifier
SUM_GAMMA2 = zeros(1,T);
TRAIN_ERROR = zeros(1,T);
TEST_ERROR = zeros(1,T);

% AdaBoost rounds
for t=1:T
    % find best weak learner and its error
    [d_index_t,thresh_t,parity_t,error_t] = findBestWeakLearner(train_data,train_labels,Dt(:,t));
    if (error_t > 0.5)
        fprintf('error at %d round > 0.5 \n',t);
        break;
    end
    
    % find the weight of h_t(x)
    alpha_t = 0.5*log((1-error_t)/error_t);
    
    % store stage t outputs
    index(t) = d_index_t;
    threshold(t) = thresh_t;
    parity(t) = parity_t;
    alpha(t) = alpha_t;
    
    % Update the weights for next round
    if(t<T)
        Zt = 0;
        for i=1:N
            h_t = sign((train_data(i,d_index_t)-thresh_t)*parity_t);
            Dt(i,t+1) = Dt(i,t)*exp(-alpha_t*train_labels(i)*h_t);
            Zt = Zt + Dt(i,t+1);
        end
        % normalize the weights
        Dt(:,t+1) = Dt(:,t+1)./Zt;
    end
    
    % Training error of Strong Classifier upto stage t
    gamma(t) = 0.5 - error_t;
    SUM_GAMMA2(t) = sum(power(gamma(1:t),2));
    TRAIN_ERROR(t) = exp(-2*SUM_GAMMA2(t));
    fprintf('Train Error of Strong Classifier upto stage %d is %d \n',t,TRAIN_ERROR(t));
    
    % Test error of Strong Classifier upto stage t
    % TEST_ERROR = Fraction of misclassified test samples
    for i=1:size(test_data,1);
        sum_wt_weak = 0;
        for tmp=1:t
            ht = sign((test_data(i,index(tmp))-threshold(tmp))*parity(tmp));
            sum_wt_weak = sum_wt_weak + alpha(tmp)*ht;
        end
        Ht = sign(sum_wt_weak);
        if(Ht ~= test_labels(i))
            TEST_ERROR(t) = TEST_ERROR(t) + 1;
        end
    end
    % normalize
    TEST_ERROR(t) = TEST_ERROR(t)./size(test_data,1);
    fprintf('Test Error of Strong Classifier upto stage %d is %d \n',t,TEST_ERROR(t));
    
end

% Plot Train Error & Test Error vs no of adaboost rounds
figure(1);
plot(TRAIN_ERROR,'b');
hold on;
plot(TEST_ERROR,'g');
title('Error vs no of rounds(T)');
xlabel('no of rounds(T)'); ylabel('Error');
legend('train','test');
hold off;

if(set_no ~= 5)
    % Final Strong Classifier on Test Data
    for i=1:size(test_data,1);
        sum_wt_weak = 0;
        for t=1:T
            ht = sign((test_data(i,index(t))-threshold(t))*parity(t));
            sum_wt_weak = sum_wt_weak + alpha(t)*ht;
        end
        Ht = sign(sum_wt_weak);
        figure(2);
        if(Ht==1)
            scatter(test_data(i,1),test_data(i,2),'+','b'); %axis([-6 6 -5 5]);
            hold on;
        else
            scatter(test_data(i,1),test_data(i,2),'*','g'); %axis([-6 6 -5 5]);
            hold on;
        end
    end
    title('Output of the Final Strong Classifier after T=40 stages on 1000 test samples');
    xlabel('x'); ylabel('y');
end
