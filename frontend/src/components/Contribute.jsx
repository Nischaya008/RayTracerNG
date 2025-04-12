import { motion } from 'framer-motion';
import { useInView } from 'react-intersection-observer';
import { FaGithub, FaCode, FaBug, FaBook } from 'react-icons/fa';

const contributionSteps = [
  {
    icon: <FaGithub className="w-6 h-6" />,
    title: 'Fork the Repository',
    description: 'Start by forking the RayTracerNG repository to your GitHub account.',
  },
  {
    icon: <FaCode className="w-6 h-6" />,
    title: 'Create a Branch',
    description: 'Create a feature branch for your changes using a descriptive name.',
  },
  {
    icon: <FaBug className="w-6 h-6" />,
    title: 'Make Changes',
    description: 'Implement your changes following our coding standards and guidelines.',
  },
  {
    icon: <FaBook className="w-6 h-6" />,
    title: 'Submit PR',
    description: 'Create a pull request with a clear description of your changes.',
  },
];

const CodeSnippet = () => (
  <div className="glass p-6 rounded-xl font-mono text-sm overflow-x-auto">
    <pre className="text-gray-300">
      <code>{`# Clone the repository
git clone https://github.com/Nischaya008/RayTracerNG
cd RayTracerNG

# Create a new branch
git checkout -b feature/your-feature-name

# Build the project
./build.bat

# Run tests
./build/RayTracerNG.exe --test`}</code>
    </pre>
  </div>
);

const StepCard = ({ icon, title, description, index }) => {
  const [ref, inView] = useInView({
    triggerOnce: true,
    threshold: 0.1,
  });

  return (
    <motion.div
      ref={ref}
      initial={{ opacity: 0, y: 50 }}
      animate={inView ? { opacity: 1, y: 0 } : { opacity: 0, y: 50 }}
      transition={{ duration: 0.5, delay: index * 0.1 }}
      className="glass p-6 rounded-xl"
    >
      <div className="text-accent mb-4">{icon}</div>
      <h3 className="text-xl font-display font-bold mb-2">{title}</h3>
      <p className="text-gray-300 text-sm">{description}</p>
    </motion.div>
  );
};

const Contribute = () => {
  return (
    <section id="contribute" className="section bg-secondary">
      <div className="container">
        <motion.div
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.5 }}
          viewport={{ once: true }}
          className="text-center mb-16"
        >
          <h2 className="text-4xl md:text-5xl font-display font-bold mb-4">
            Contribute
          </h2>
          <p className="text-gray-300 text-lg">
            Join me in building the future of ray tracing
          </p>
        </motion.div>

        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-8 mb-16">
          {contributionSteps.map((step, index) => (
            <StepCard key={index} {...step} index={index} />
          ))}
        </div>

        <div className="max-w-4xl mx-auto">
          <motion.div
            initial={{ opacity: 0, y: 20 }}
            whileInView={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.5 }}
            viewport={{ once: true }}
            className="text-center mb-8"
          >
            <h3 className="text-2xl font-display font-bold mb-4">
              Quick Start Guide
            </h3>
            <p className="text-gray-300 mb-6">
              Follow these steps to get started with development
            </p>
          </motion.div>

          <CodeSnippet />

          <motion.div
            initial={{ opacity: 0, y: 20 }}
            whileInView={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.5, delay: 0.2 }}
            viewport={{ once: true }}
            className="text-center mt-8"
          >
            <a
              href="https://github.com/Nischaya008/RayTracerNG"
              className="btn btn-primary inline-flex items-center gap-2"
              target="_blank"
              rel="noopener noreferrer"
            >
              <FaGithub />
              View on GitHub
            </a>
          </motion.div>
        </div>
      </div>
    </section>
  );
};

export default Contribute; 